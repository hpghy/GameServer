/***************************************************************************
 *
 * Copyright (c) 2018 hpghy, Inc. All Rights Reserved
 * $Id$
 *
 **************************************************************************/

/**
* @file kcp_client.cpp
* @author hpghy(790042744@qq.com)
* @date 2018/11/17 21:23:03
* @version $Revision$
* @brief
*
**/

#include "connection/kcp_client.h"

#include <boost/date_time/posix_time/posix_time.hpp>
#include "service/rpc_service.h"
#include "util/util.h"
#include "service/request_queue.h"
#include "server/server.h"

const uint32_t KcpClient::reconnect_max_interval = 12;

static const unsigned int CONN_REQ_LEN = 16;
static const char CONN_REQ_SYNC = 'S';
static const char CONN_REQ_ACK = 'A';

KcpClient::KcpClient(boost::asio::io_service& io_service, const std::string& ip,  uint32_t port)
    : KcpConnection(io_service, ServerWptr()),
      reconnect_timer_(io_service)
{
    boost::asio::ip::address addr;
    addr.from_string(ip);
    remote_addr_ = udp_addr(addr, port);
    setRemoteAddr(remote_addr_);
    std::cout << "Create KcpClient: " << getRemoteAddr();
}

// 主线程执行,使用strand保护
void KcpClient::asyncConnect()
{
    DEBUG_LOG << "asyncConnect....";
    auto self = std::dynamic_pointer_cast<KcpClient>(shared_from_this());
    getKcpStrand()->post([self]() { self->doAsyncConnect(); });
}

void KcpClient::doAsyncConnect()
{
    if (status_ == ConnStatus::STATUS_CONNECTING)
    {
        return;
    }
    if (!connect_cb_)
    {
        ERROR_LOG << "No connect_cb";
        return;
    }
    doConnect();
}

void KcpClient::doConnect()
{
    status_ = ConnStatus::STATUS_CONNECTING;
    boost_err ec;
    getSocket().connect(remote_addr_, ec);
    if (ec)
    {
        WARN_LOG << "connect ec value: " << ec.value() << " message: " << ec.message();
        onConnected(false);
        return;
    }

    DEBUG_LOG << "HPTEST directSendData CONN_REQ_SYNC";
    if (!directSendData(&CONN_REQ_SYNC, 1))
    {
        onConnected(false);
        return;
    }

    // 不能使用基类的asyncReceive
    asyncReceiveSync();
}

bool KcpClient::directSendData(const char* data, std::size_t bytes)
{
    if (0 == bytes)
    {
        return true;
    }
    boost_err ec;
    getSocket().send(boost::asio::buffer(data, bytes), 0, ec);
    if (ec)
    {
        ERROR_LOG << "socket.send failed ec: " << ec.value() << " message: " << ec.message();
        return false;
    }
    return true;
}

void KcpClient::asyncReceiveSync()
{
    auto self = std::dynamic_pointer_cast<KcpClient>(shared_from_this());
    auto handle = [self](const boost_err & ec, std::size_t bytes) { self->handleReceiveSync(ec, bytes); };
    getSocket().async_receive(boost::asio::buffer(getRecvBuffer()), getKcpStrand()->wrap(handle));
}

void KcpClient::handleReceiveSync(const boost_err& ec, std::size_t bytes)
{
    DEBUG_LOG << "handleReceiveSync bytes: " << bytes;
    if (ec)
    {
        ERROR_LOG << "handleReceiveSync ec: " << ec.value() << " message: " << ec.message();
        return;
    }
    if (status_ != ConnStatus::STATUS_CONNECTING)
    {
        WARN_LOG << "status_ != ConnStatus::STATUS_CONNECTING!";
        return;
    }

    char* buf = getRecvBuffer().data();
    if (CONN_REQ_LEN == bytes && CONN_REQ_SYNC == buf[0])
    {
        DEBUG_LOG << "KcpClient rece server Sync";
        buf[0] = CONN_REQ_ACK;
        if (!directSendData(buf, CONN_REQ_LEN))
        {
            ERROR_LOG << "directSendData failed";
            return;
        }
        asyncReceiveSync();
        return;
    }
    if (1 == bytes && CONN_REQ_ACK == buf[0])
    {
        DEBUG_LOG << "KcpClient rece server Ack";
        onConnected(true);
        return;
    }
}

void KcpClient::onConnected(bool is_connected)
{
    DEBUG_LOG << "onConnected is_connected: " << is_connected;
    status_ = is_connected ? ConnStatus::STATUS_CONNECTED : ConnStatus::STATUS_DISCONNECTED;
    onConnected();
}

void KcpClient::onConnected()
{
    bool connected = (ConnStatus::STATUS_CONNECTED == status_);
    INFO_LOG << "KcpClient onConnected " << connected;
    auto self = std::static_pointer_cast<KcpClient>(shared_from_this());
    auto callback = [self, connected](void)
    {
        // 主线程执行callback
        if (!self->connect_cb_)
        {
            ERROR_LOG << "No connect_cb";
            return;
        }
        if (connected)
        {
            self->startWork();
        }
        self->connect_cb_(self, connected);
    };
    QueueRequest* request = new QueueCallbackRequest(callback);
    RequestQueueInst::instance().pushRequest(request);
}

// TODO...继承函数没有设计好, 需要修改
// io线程里面执行,使用strand保护
void KcpClient::handleCloseSocket()
{
    INFO_LOG << "KcpCLient handleCloseSocket: " << getRemoteAddr();
    if (getSocket().is_open())
    {
        boost_err err;
        getSocket().close(err);
        if (err)
        {
            ERROR_LOG << "KcpClient close socket error " << err.message() \
                      << " value: " << err.value();
        }
        onDisconnected();
    }
    cancelKcpUpdateTimer();
}

void KcpClient::onDisconnected()
{
    INFO_LOG << "KcpClient: onDisconnected " << getRemoteAddr();
    auto self = std::dynamic_pointer_cast<KcpClient>(shared_from_this());
    auto callback = [self](void)
    {
        // 主线程中执行
        if (self->disconnect_cb_)
        {
            self->disconnect_cb_(self);
        }
        else
        {
            ERROR_LOG << "No disconnect_cb";
        }
    };
    QueueRequest* request = new QueueCallbackRequest(callback);
    RequestQueueInst::instance().pushRequest(request);
}


/* vim: set ts=4 sw=4 sts=4 tw=100 */
