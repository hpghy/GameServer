/***************************************************************************
 *
 * Copyright (c) 2018 hpghy, Inc. All Rights Reserved
 * $Id$
 *
 **************************************************************************/

/**
* @file kcp_server.cpp
* @author hpghy(790042744@qq.com)
* @date 2018/07/29 14:10:22
* @version $Revision$
* @brief
*
**/

#include "server/kcp_server.h"

#include <boost/bind.hpp>
#include "service/request_queue.h"
#include "server/mobile_server.h"

//必须小于KCP_OVERHEADER
static const unsigned int CONN_REQ_LEN = 16;
static const char CONN_REQ_SYNC = 'S';
static const char CONN_REQ_ACK = 'A';
static const uint64_t CONN_REQ_TIMEOUT = 5000;

KcpServer::KcpServer(boost::asio::io_service& io_service)
    : Server(io_service),
      socket_(io_service)
{
    INFO_LOG << "Create kcp_server";
}

KcpServer::~KcpServer()
{
    INFO_LOG << "Destroy kcp_server";
}

void KcpServer::bind(const std::string& ip, uint16_t port)
{
    Server::bind(ip, port);
    bind_addr_ = boost::asio::ip::udp::endpoint(
                     boost::asio::ip::address::from_string(ip), port);
    boost::system::error_code ec;
    socket_.open(bind_addr_.protocol(), ec);
    if (ec)
    {
        ERROR_LOG << "kcpserver open failed: " << ec.value() << " " << ec.message();
    }

    setOption();

    socket_.bind(bind_addr_, ec);
    if (ec)
    {
        ERROR_LOG << "kcpserver bind failed: " << ec.value() << " " << ec.message();
    }
}

void KcpServer::listen()
{
    asyncReceive();
}

void KcpServer::start()
{
    Server::start();
}

void KcpServer::stop()
{
    Server::stop();
    if (socket_.is_open())
    {
        boost_err err;
        socket_.close(err);
        if (err)
        {
            ERROR_LOG << "kcp_server close socket error " << err.value()
                      << " " << err.message();
        }
    }
}

void KcpServer::asyncReceive()
{
    socket_.async_receive_from(boost::asio::buffer(recv_buffer_),
                               rmt_addr_,
                               boost::bind(&KcpServer::handleReceive,
                                           std::static_pointer_cast<KcpServer>(shared_from_this()),
                                           boost::asio::placeholders::error,
                                           boost::asio::placeholders::bytes_transferred));
}

void KcpServer::handleReceive(const boost::system::error_code& ec,
                              std::size_t bytes)
/**
 * 大体上模拟TCP的三次握手步骤
 **/
{
    DEBUG_LOG << "KcpServer::handle_receive read bytes " << bytes << " from " << rmt_addr_;
    if (ec)
    {
        INFO_LOG << "KcpServer::handle_receive " << ec.value() << " " << ec.message();
        return;
    }

    const char* buf = recv_buffer_.data();
    if (bytes >= KCP_OVERHEAD)
    {
        // KCP报文不应该到这里来
        ERROR_LOG << "KcpServer rece one kcp segment from " << rmt_addr_;
        asyncReceive();
        return;
    }

    if (buf[0] == CONN_REQ_SYNC && 1 == bytes)
    {
        handleSyncRequest();
    }
    else if (buf[0] == CONN_REQ_ACK && CONN_REQ_LEN == bytes)
    {
        handleAckRequest();
    }
    else
    {
        WARN_LOG << "KcpServer not support CONN_REQ type: " << buf[0];
    }

    asyncReceive();
}

void KcpServer::handleSyncRequest()
{
    DEBUG_LOG << "KcpServer handleSyncRequest: " << rmt_addr_;
    static char sync_data[CONN_REQ_LEN];

    // S + randint32t + timestamp + randint32t(no use)
    sync_data[0] = CONN_REQ_SYNC;

    uint32_t rand0 = rand();
    *reinterpret_cast<uint32_t*>(sync_data + 1) = rand0;

    auto sync_stamp = stamp_millisecond();
    *reinterpret_cast<uint64_t*>(sync_data + 5) = sync_stamp;
    DEBUG_LOG << "sync_stamp: " << rmt_addr_ << " " << sync_stamp;

    uint32_t rand1 = rand();
    *reinterpret_cast<uint32_t*>(sync_data + 13) = rand1;

    boost_err ec;
    socket_.send_to(boost::asio::buffer(sync_data, sizeof(sync_data)), rmt_addr_, 0, ec);
    if (ec)
    {
        WARN_LOG << "send sync_data failed: " << ec.message() << " value: " << ec.value();
    }
}

void KcpServer::handleAckRequest()
{
    DEBUG_LOG << "KcpServer handleAckRequest: " << rmt_addr_;

    const char* buf = recv_buffer_.data();
    uint64_t sync_stamp = *reinterpret_cast<const uint64_t*>(buf + 5);
    auto now = stamp_millisecond();
    if (sync_stamp > now || now - sync_stamp > CONN_REQ_TIMEOUT)
    {
        WARN_LOG << "KcpServer sync request timeout: " << rmt_addr_ << " " << sync_stamp << " " << " now: " << now;
        return;
    }

    // TODO...注意处理创建了两个相同rmt_addr连接的情况

    // create new connection
    INFO_LOG << "cteate new kcp_connection from " << rmt_addr_;

    auto conn = std::make_shared<KcpConnection>(getIoService(), shared_from_this());
    conn->bindConnect(bind_addr_, rmt_addr_);
    // 新连接向client发送Ack确认连接成功, 此时socket尚未ioservice，线程安全
    conn->syncSend(&CONN_REQ_ACK, 1);

    // 也可以在ServiceManager对connection集合加锁，没有必要把callback传递给主线程
    auto callback = [conn](void)
    {
        // execute in main thread, and conn_ptr_ is new object, not old one!
        conn->onConnected();
        conn->startWork();
    };
    QueueRequest* request = new QueueCallbackRequest(callback);
    RequestQueueInst::instance().pushRequest(request);
}

void KcpServer::setOption()
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
        ERROR_LOG << "setsockopt SO_REUSEADDR|SO_REUSEPORT failed: " << ret << std::endl;
    }
#endif

    boost::system::error_code ec;
    socket_.non_blocking(true, ec);
    if (ec)
    {
        ERROR_LOG << "non_blocking failed: " << ec.message();
    }
    // TODO... set send and recv buffer size
}


/* vim: set ts=4 sw=4 sts=4 tw=100 */
