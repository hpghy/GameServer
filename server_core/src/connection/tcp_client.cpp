/***************************************************************************
 *
 * Copyright (c) 2018 hpghy, Inc. All Rights Reserved
 * $Id$
 *
 **************************************************************************/

/**
* @file tcp_client.cpp
* @author hpghy(790042744@qq.com)
* @date 2018/07/29 14:20:27
* @version $Revision$
* @brief
*
**/

#include "connection/tcp_client.h"

#include <boost/date_time/posix_time/posix_time.hpp>
#include "service/rpc_service.h"
#include "util/util.h"
#include "service/request_queue.h"
#include "server/server.h"

const uint32_t TcpClient::reconnect_max_interval = 12;

TcpClient::TcpClient(boost::asio::io_service& io_service, const std::string& ip, uint32_t port)
    : TcpConnection(io_service, ServerWptr()),
      reconnect_timer_(io_service)
{
    boost::asio::ip::address addr;
    addr.from_string(ip);
    remote_addr_ = tcp_addr(addr, port);
    setRemoteAddr(remote_addr_);
    INFO_LOG << "Create TcpClient: " << getRemoteAddr();
}

// 主线程执行,使用strand保护
void TcpClient::asyncConnect()
{
    DEBUG_LOG << "asyncConnect....";
    auto self = std::dynamic_pointer_cast<TcpClient>(shared_from_this());
    getSendStrand()->post([self]() { self->doAsyncConnect(); });
}

// io线程执行
void TcpClient::doAsyncConnect()
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
    status_ = ConnStatus::STATUS_CONNECTING;
    auto self = std::dynamic_pointer_cast<TcpClient>(shared_from_this());
    getSocket().async_connect(remote_addr_, getSendStrand()->wrap([self](const boost_err & ec) { self->handleConnect(ec); }));
}

// io线程里面执行
void TcpClient::handleConnect(const boost_err& ec)
{
    if (!ec)
    {
        status_ = ConnStatus::STATUS_CONNECTED;
        onConnected();
        return;
    }

    WARN_LOG << "TcpClient connect " << getRemoteAddr() << " failed: " << ec.value() << " " << ec.message() << std::endl;
    status_ = ConnStatus::STATUS_DISCONNECTED;

    reconnect_timer_.expires_from_now(boost::posix_time::seconds(reconnect_interval_));
    auto self = std::dynamic_pointer_cast<TcpClient>(shared_from_this());
    reconnect_timer_.async_wait(getSendStrand()->wrap([self](const boost_err & ec) { self->reconnectTimer(ec); }));
    reconnect_interval_ = std::min(reconnect_max_interval, reconnect_interval_ * 2);
}

// io线程执行, 使用getSendStrand()保护
void TcpClient::reconnectTimer(const boost::system::error_code& ec)
{
    DEBUG_LOG << "reconnectTimer: " << getRemoteAddr();
    if (ec)
    {
        WARN_LOG << "reconnectTimer ec: " << ec.value() << " msg: " << ec.message();
        return;
    }
    status_ = ConnStatus::STATUS_CONNECTING;
    boost::system::error_code error;
    getSocket().close(error);
    if (error)
    {
        WARN_LOG << "close error: " << error.message();
    }
    auto self = std::dynamic_pointer_cast<TcpClient>(shared_from_this());
    getSocket().async_connect(remote_addr_, getSendStrand()->wrap([self](const boost_err & ec) { self->handleConnect(ec); }));
}

void TcpClient::onConnected()
{
    bool connected = (ConnStatus::STATUS_CONNECTED == status_);
    INFO_LOG << "TcpClient onConnected " << connected;
    auto self = std::static_pointer_cast<TcpClient>(shared_from_this());
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

// io线程里面执行,使用strand保护
void TcpClient::handleCloseSocket()
{
    INFO_LOG << "TcpCLient handleCloseSocket: " << getRemoteAddr();
    if (getSocket().is_open())
    {
        boost_err err;
        getSocket().close(err);
        if (err)
        {
            ERROR_LOG << "TcpClient close socket error " << err.message() \
                      << " value: " << err.value();
        }
        onDisconnected();
    }
}

void TcpClient::onDisconnected()
{
    INFO_LOG << "TcpClient: onDisconnected " << getRemoteAddr();
    auto self = std::static_pointer_cast<TcpClient>(shared_from_this());
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
