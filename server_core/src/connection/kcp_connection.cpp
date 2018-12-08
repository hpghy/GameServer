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

KcpConnection::KcpConnection(boost::asio::io_service& io_service, std::weak_ptr<Server> server_wptr)
    : Connection(io_service, server_wptr),
      socket_(io_service),
      kcp_strand_(std::make_shared<boost::asio::strand>(io_service)),
      kcp_(nullptr),
      kcp_timer_(io_service)
{}

KcpConnection::~KcpConnection()
{
    INFO_LOG << "destroy KcpConnection " << rmt_addr_ << std::endl;
    releaseKcp();
}

// KcpServer在IO线程中执行
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

// 主线程调用的
void KcpConnection::startWork()
{
    auto self = std::dynamic_pointer_cast<KcpConnection>(shared_from_this());
    kcp_strand_->post([self]() { self->doStartWork(); });
}

// IO线程中执行
void KcpConnection::doStartWork()
{
    last_send_millsec_ = last_recv_millsec_ = stamp_millisecond();
    createKcp();
    startKcpUpdateTimer();       // ==> updateCheckKcpTimer();
    asyncReceive();
}

// ikcp内部发送时真正调用的接口,保证在IO线程中执行
void KcpConnection::syncSend(const char* buff, std::size_t len)
{
    boost_err ec;
    socket_.send(boost::asio::buffer(buff, len), 0, ec);
    last_send_millsec_ = stamp_millisecond();
    if (ec)
    {
        closeSocket();
        ERROR_LOG << "socket_.send error: " << ec.message() << " value: " << ec.value() << std::endl;
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

    // 设置socket发送接收缓存区大小
    boost::asio::socket_base::send_buffer_size send_option(snd_buf_size_ * 1024);
    socket_.set_option(send_option);

    boost::asio::socket_base::receive_buffer_size recv_option(snd_buf_size_ * 1024);
    socket_.set_option(recv_option);
}

void KcpConnection::asyncReceive()
{
    auto self = std::dynamic_pointer_cast<KcpConnection>(shared_from_this());
    socket_.async_receive(boost::asio::buffer(recv_buffer_),
                          kcp_strand_->wrap([self](const boost_err & ec, std::size_t bytes)
    {
        self->handleReceive(ec, bytes);
    }));
}

// rcv_strand保护相关读的资源
void KcpConnection::handleReceive(const boost_err& ec, std::size_t bytes)
{
    DEBUG_LOG << "KcpConnection::handle_receive " << bytes << std::endl;
    if (ec)
    {
        ERROR_LOG << "handleReceive ec: " << ec.value() << " message: " << ec.message();
        closeSocket();
        return;
    }

    auto now = stamp_millisecond();
    last_recv_millsec_ = now;

    if (bytes < KCP_OVERHEAD)
    {
        if (bytes == heartbeat_str.size() && 0 == strncmp(recv_buffer_.data(), heartbeat_str.data(), bytes))
        {
            DEBUG_LOG << rmt_addr_ << " receive heartbeat!" << std::endl;
        }
        if (0 == bytes)
        {
            INFO_LOG << "Read zero bytes from socket, and closeSocket\n";
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

    auto channel_ptr = getChannel();
    if (!channel_ptr)
    {
        ERROR_LOG << "tcp_connection channel_ptr is null\n";
        closeSocket();
        return;
    }
    while ((ret = ikcp_recv(kcp_, recv_buffer_.data(), recv_buffer_.max_size())) > 0)
    {
        if (!channel_ptr->handleData(recv_buffer_.data(), ret))
        {
            ERROR_LOG << "handle_data failed...." << std::endl;
            closeSocket();
        }
    }
    if (ret == -3)
    {
        ERROR_LOG << "recv_buffer_ is too small ikcp_recv return -3!!!" << std::endl;
    }

    ikcp_update(kcp_, now);		//强制flush,ack无延迟的发送出去
    asyncReceive();
}

int32_t KcpConnection::asyncSend(std::shared_ptr<std::string> data)
{
    if (isClosed())
    {
        WARN_LOG << "socket has been closed";
        return -1;
    }
    auto self = std::dynamic_pointer_cast<KcpConnection>(shared_from_this());
    kcp_strand_->post([self, data]() { self->doAsyncSendData(data); });
    return 0;
}

void KcpConnection::doAsyncSendData(std::shared_ptr<std::string> data)
{
    //直接往kcp里面写，kcp自己会调用output接口
    int ret = ikcp_send(kcp_, reinterpret_cast<const char*>(data->c_str()), data->size());
    if (ret < 0)
    {
        WARN_LOG << "ikcp_send return " << ret << std::endl;
        return;
    }
    auto now = stamp_millisecond();
    ikcp_update(kcp_, now);		//强制flush,ack无延迟的发送出去
}

void KcpConnection::disconnect()
{
    auto self = std::dynamic_pointer_cast<KcpConnection>(shared_from_this());
    kcp_strand_->post([self]() { self->doDisconnect(); });
}

void KcpConnection::dispatchCloseSocket()
{
    auto self = std::dynamic_pointer_cast<KcpConnection>(shared_from_this());
    kcp_strand_->post([self] { self->handleCloseSocket(); });
}

void KcpConnection::handleCloseSocket()
{
    cancelKcpUpdateTimer();
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

void KcpConnection::setInterval(uint32_t interval)
{
    if (interval > 0)
    {
        interval_ = interval;
    }
}

void KcpConnection::setResend(uint32_t resend)
{
    if (resend > 0)
    {
        resend_ = resend;
    }
}

void KcpConnection::setSndwnd(uint32_t sndwnd)
{
    if (sndwnd > 0)
    {
        sndwnd_ = sndwnd;
    }
}

void KcpConnection::setRcvwnd(uint32_t rcvwnd)
{
    if (rcvwnd > 0)
    {
        rcvwnd_ = rcvwnd;
    }
}

void KcpConnection::setSndBufferSize(uint32_t snd_size)
{
    if (snd_size > 0)
    {
        snd_buf_size_ = snd_size;
    }
}

void KcpConnection::setRcvBufferSize(uint32_t rcv_size)
{
    if (rcv_size > 0)
    {
        rcv_buf_size_ = rcv_size;
    }
}

bool KcpConnection::createKcp()
{
    if (kcp_)
    {
        return false;
    }
    kcp_ = ikcp_create(0, (void*)this);
    // 开启快速模式
    ikcp_nodelay(kcp_, 1, interval_, resend_, nocwnd_);
    ikcp_wndsize(kcp_, sndwnd_, rcvwnd_);
    kcp_->output = kcp_sync_output;
    kcp_->stream = stream_;
    return true;
}

void KcpConnection::releaseKcp()
{
    if (!kcp_)
    {
        return;
    }
    ikcp_release(kcp_);
    kcp_ = nullptr;
}

// 启动定时更新kcp的定时器
void KcpConnection::startKcpUpdateTimer()
{
    if (is_kcp_updating == true)
    {
        return;
    }
    is_kcp_updating = true;
    updateKcp();
}

void KcpConnection::updateKcp()
{
    auto now = stamp_millisecond();
    if (now - last_recv_millsec_ > timeout_millsec_)
    {
        WARN_LOG << "now - last_recv_millsec_ > timeout_millsec_\n" << std::endl;
        closeSocket();
        return;
    }

    ikcp_update(kcp_, now);
    auto next_update_time = ikcp_check(kcp_, now);
    auto delay = interval_;
    if (next_update_time > now && next_update_time - now > 2)
    {
        delay = next_update_time - now;
    }

    kcp_timer_.expires_from_now(boost::posix_time::milliseconds(delay));
    auto self = std::dynamic_pointer_cast<KcpConnection>(shared_from_this());
    kcp_timer_.async_wait(kcp_strand_->wrap([self](const boost_err & ec) { self->onUpdateKcp(ec); }));

    // TODO...有没有必要自己发送心跳
    if (now - last_send_millsec_ > tick_millsec_)
    {
        DEBUG_LOG << "send heartbeat to remote peer!";
        sendHeartbeat();
    }
}

void KcpConnection::onUpdateKcp(const boost_err& ec)
{
    if (ec)
    {
        WARN_LOG << " failed value: " << ec.value() << " message: " << ec.message();
        return;
    }
    if (is_kcp_updating)       // 定时器没有被取消
    {
        updateKcp();
    }
}


/* vim: set ts=4 sw=4 sts=4 tw=100 */
