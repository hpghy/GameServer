/***************************************************************************
 *
 * Copyright (c) 2018 hpghy, Inc. All Rights Reserved
 * $Id$
 *
 **************************************************************************/

/**
* @file kcp_connection.cpp
* @author hpghy(790042744@qq.com)
* @date 2018/07/29 14:19:13
* @version $Revision$
* @brief
*
**/

#include "connection/kcp_connection.h"

#include <arpa/inet.h>
#include <functional>
#include <boost/date_time/posix_time/posix_time.hpp>
#include "connection/rpc_channel.h"
#include "util/util.h"
#include "service/service_manager.h"
#include "server/server.h"

const std::string KcpConnection::heartbeat_str = "tick";

KcpConnection::KcpConnection(boost::asio::io_service& io_service, std::weak_ptr<Server> server)
    : Connection(io_service, server),
      socket_(io_service),
      kcp_(nullptr),
      kcp_timer_(io_service)
{
}

KcpConnection::~KcpConnection()
{
    INFO_LOG << "destroy KcpConnection " << rmt_addr_ << std::endl;
    releaseKcp();
}

void KcpConnection::bindConnect(const udp_addr& bind,
                                const udp_addr& rmt)
{
    bind_addr_ = bind;
    setRemoteAddr(rmt);
    rmt_addr_ = rmt;

    boost::system::error_code ec;
    socket_.open(bind_addr_.protocol(), ec);
    if (ec)
    {
        ERROR_LOG << "open failed for " << ec.value() << " " << ec.message() << std::endl;
        return;
    }

    setOption();

    socket_.bind(bind_addr_, ec);
    if (ec)
    {
        ERROR_LOG << "bind failed for " << ec.value() << " " << ec.message() << std::endl;
        return;
    }

    socket_.connect(rmt_addr_, ec);
    if (ec)
    {
        ERROR_LOG << "connect to " << rmt_addr_ << " failed!" << std::endl;
        return;
    }
}

void KcpConnection::doStartWork()
{
    last_recv_ = last_send_ = stamp_millisecond();
    createKcp();
    startKcpUpdate();       // ==> updateCheckKcpTimer();
    asyncReceive();
}

void KcpConnection::syncSend(const char* buff, std::size_t len)
{
    boost_err ec;
    socket_.send(boost::asio::buffer(buff, len), 0, ec);
    if (ec)
    {
        closeSocket();
        ERROR_LOG << "socket_.send error: " << ec.message() << " value: " << ec.value() << std::endl;
    }
    else
    {
        last_send_ = stamp_millisecond();
    }
}

void KcpConnection::setOption()
{
#ifdef __APPLE__
    boost::asio::socket_base::reuse_address option(true);
    socket_.set_option(option);
#else
    //boost asio 目前只支持reuse_addr
    int one = 1;
    int ret = setsockopt(socket_.native_handle(), SOL_SOCKET, SO_REUSEADDR | SO_REUSEPORT, &one, sizeof(one));
    if (ret != 0)
    {
        std::cerr << "setsockopt SO_REUSEADDR|SO_REUSEPORT failed: " << ret << std::endl;
    }
#endif

    boost::system::error_code ec;
    socket_.non_blocking(true, ec);
    if (ec)
    {
        ERROR_LOG << "non_blocking failed: " << ec.message();
    }
    //TODO...设置socket发送接收缓存区大小
}

void KcpConnection::asyncReceive()
{
    socket_.async_receive(boost::asio::buffer(recv_buffer_),
                          getRecvStrand()->wrap(std::bind(&KcpConnection::handleReceive,
                                  std::static_pointer_cast<KcpConnection>(shared_from_this()),
                                  std::placeholders::_1,
                                  std::placeholders::_2)));
}

// rcv_strand保护相关读的资源
void KcpConnection::handleReceive(const boost_err& ec, std::size_t bytes)
{
    DEBUG_LOG << "KcpConnection::handle_receive " << bytes << std::endl;
    if (bytes < KCP_OVERHEAD)
    {
        std::size_t size = heartbeat_str.size();
        if (bytes == size && 0 == strncmp(recv_buffer_.data(), heartbeat_str.data(), bytes))
        {
            DEBUG_LOG << rmt_addr_ << " receive heartbeat!" << std::endl;
        }
        if (bytes == 0)
        {
            DEBUG_LOG << "Read zero bytes from socket, and closeSocket\n";
            closeSocket();
        }
        else
        {
            asyncReceive();
        }
        return;
    }

    int ret = ikcp_input(kcp_, recv_buffer_.data(), bytes);
    if (ret < 0)
    {
        INFO_LOG << "ikcp_input ret: " << ret << " bytes: " << bytes << std::endl;
        asyncReceive();
        return;
    }

    auto channel = getChannel().lock();
    if (!channel)
    {
        ERROR_LOG << "tcp_connection channel_ptr is null\n";
        return;
    }
    while ((ret = ikcp_recv(kcp_, recv_buffer_.data(), recv_buffer_.max_size())) > 0)
    {
        if (!channel->handleData(recv_buffer_.data(), ret))
        {
            ERROR_LOG << "handle_data failed...." << std::endl;
            closeSocket();
        }
    }
    if (ret == -3)
    {
        ERROR_LOG << "recv_buffer_ is too small ikcp_recv return -3!!!" << std::endl;
    }

    auto now = stamp_millisecond();
    last_recv_ = now;
    ikcp_update(kcp_, now);		//强制flush,ack无延迟的发送出去
    asyncReceive();
}

// send_strand保护资源
void KcpConnection::doAsyncSendData(std::shared_ptr<std::string> data)
{
    //直接往kcp里面写，kcp自己会调用output接口
    int ret = ikcp_send(kcp_,
                        reinterpret_cast<const char*>(data->c_str()),
                        data->size());
    if (ret < 0)
    {
        INFO_LOG << "ikcp_send return " << ret << std::endl;
    }
    else
    {
        auto now = stamp_millisecond();
        ikcp_update(kcp_, now);		//强制flush,ack无延迟的发送出去
    }
}

void KcpConnection::updateKcp()
{
    auto now = stamp_millisecond();
    if (now - last_recv_ > timeout_mill_)
    {
        INFO_LOG << "now - last_recv_ > timeout_mill_\n" << std::endl;
        closeSocket();
        return;
    }

    ikcp_update(kcp_, now);
    auto delay = ikcp_check(kcp_, now) - now;
    if (delay <= 1)
    {
        delay = interval_;
    }
    // TODO...注意线程不安全，io-thread: handleReceive, io-thread: handleAsyncSend, io-thread: onUpdateKcp
    // TODO...需要使用strand包裹
    kcp_timer_.expires_from_now(boost::posix_time::milliseconds(delay));
    kcp_timer_.async_wait(std::bind(&KcpConnection::onUpdateKcp,
                                    std::static_pointer_cast<KcpConnection>(shared_from_this()),
                                    std::placeholders::_1));

    if (now - last_send_ > tick_mill_)
    {
        DEBUG_LOG << "send heartbeat to remote peer!";
        sendHeartbeat();
    }
}

void KcpConnection::onUpdateKcp(const boost_err& ec)
{
    if (!ec)
    {
        if (kcp_updating)
        {
            updateKcp();
        }
        else
        {
            INFO_LOG << "kcp_updating is false....\n";
        }
    }
    else
    {
        WARN_LOG << " failed value: " << ec.value() << " message: " << ec.message();
    }
}

void KcpConnection::handleCloseSocket()
{
    cancelKcpUpdate();
    if (socket_.is_open())
    {
        boost_err err;
        socket_.close(err);
        if (err)
        {
            ERROR_LOG << "KcpConnection close socket error " << err.message() \
                      << " value: " << err.value() << std::endl;
        }
    }
}


/* vim: set ts=4 sw=4 sts=4 tw=100 */
