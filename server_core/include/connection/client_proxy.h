/***************************************************************************
 *
 * Copyright (c) 2018 hpghy, Inc. All Rights Reserved
 * $Id$
 *
 **************************************************************************/

/**
* @file client_proxy.h
* @author hpghy(790042744@qq.com)
* @date 2018/11/18 21:27:08
* @version $Revision$
* @brief
*
**/
#ifndef CLIENT_PROXY_H
#define CLIENT_PROXY_H

#include <memory>
#include <string>
#include <cstdint>
#include <boost/date_time/posix_time/posix_time.hpp>

#include "connection/tcp_client.h"
#include "connection/kcp_client.h"

struct TcpProtocol
{
    using ClientType = TcpClient;
    using ClientPtr = std::shared_ptr<TcpClient>;
};

struct KcpProtocol
{
    using ClientType = KcpClient;
    using ClientPtr = std::shared_ptr<KcpClient>;
};

struct EnetProtocol
{};

/**
 * ClientProxy的作用有两点：
 * 1. 屏蔽IO线程，只工作在主线程中，向上层提供简洁的请求连接功能；
 * 2. 屏蔽底层的tcp/udp细节, 并提供统一的功能比如重连尝试
 **/
template <typename ProtocolT>
class ClientProxy : public std::enable_shared_from_this<ClientProxy>
{
    public:
        using ClientProxyPtr = std::shared_ptr<ClientProxy>;
        using ClientPtr = typename ProtocolT::ClientPtr;
        const uint32_t reconnect_max_interval = 15;

    public:
        static std::shared_ptr<ClientPtr> create(boost::asio::io_service& io_service, const std::string& ip, uint16_t port)
        {
            return std::make_shared<ClientPtr>(io_service, ip, port);
        }

        // 0 表示无限尝试重连
        void asyncConnect(uint32_t retry_times = 0);
        void disconnect() { client_ptr_->disconnect(); }

        void setConnectCallback(std::function<void(ClientProxyPtr, bool)> connect_cb)
        {
            connect_cb_ = connect_cb;
        }
        void setDisconnectCallback(std::function<void(ClientProxyPtr)> disconnect_cb)
        {
            disconnect_cb_ = disconnect_cb;
        }

        const std::string& getRemoteAddr() const { return client_ptr_->getRemoteAddr(); };

        std::shared_ptr<RpcChannel> getChannel() { return client_ptr_->getChannel(); }

        ClientPtr getClientPtr() { return client_ptr_; }

    private:
        ClientProxy(boost::asio::io_service& io_service, const std::string& ip, uint16_t port)
            : client_ptr_(std::make_shared<ProtocolT::ClientType>(io_service, ip, port))
        {}
        ~ClientProxy()
        {
            if (timer_ptr_)
                timer_ptr_->cancel();
        }

        void onConnected(bool is_connected);
        void onDisconnected();
        void reconnectTimer();

    private:
        ClientPtr       client_ptr_;
        ConnStatus 	        status_ = ConnStatus::STATUS_DISCONNECTED;
        std::function<void(ClientProxyPtr, bool)> connect_cb_;
        std::function<void(ClientProxyPtr)> disconnect_cb_;
        uint32_t        reconnect_interval_ = 1;
        uint32_t        retry_times_ = 0;
        // 使用主线程的定时器
        ProxyHandlePtr timer_ptr_;
};
using ClientProxyPtr = std::shared_ptr<ClientProxy>;

template <typename ProtocolT>
void ClientProxy::asyncConnect(uint32_t retry_times)
{
    if (!connect_cb_ || !disconnect_cb_)
    {
        WARN_LOG << "setConnectCallback and setDisconnectCallback first!!!";
        return;
    }
    if (ConnStatus::STATUS_DISCONNECTED != status_)
    {
        WARN_LOG << "ConnStatus::STATUS_DISCONNECTED != status_";
        return;
    }
    auto self = shared_from_this();
    client_ptr_->setConnectCallback([self](ClientPtr ptr, bool is_connected)
    {
        self->onConnected(is_connected);
    });
    client_ptr_->setDisconnectCallback([self](ClientPtr) { self->onDisconnected();  });
    retry_times_ = retry_times;
    client_ptr_->asyncConnect();
}

template <typename ProtocolT>
void ClientProxy::onConnected(bool is_connected)
{
    if (!connect_cb_)
    {
        return;
    }
    if (is_connected || 0 == retry_times_)
    {
        status_ = is_connected ? ConnStatus::STATUS_CONNECTED : ConnStatus::STATUS_DISCONNECTED;
        connect_cb_(shared_from_this(), is_connected);
        return;
    }
    // 继续尝试重连
    -- retry_times_;
    if (timer_ptr_)
    {
        timer_ptr_->cancel();
    }
    auto self = shared_from_this();
    timer_ptr_ = TimerManagerInst::instance().addTimer(1, [self]() { self->reconnnectTimer(); });
}

template <typename ProtocolT>
void ClientProxy::reconnectTimer()
{
    // TODO...
}

template <typename ProtocolT>
void ClientProxy::onDisconnected()
{
    if (!disconnect_cb_)
    {
        return;
    }
    status_ = ConnStatus::STATUS_DISCONNECTED;
    disconnect_cb_(shared_from_this());
}

#endif  // CLIENT_PROXY_H

/* vim: set ts=4 sw=4 sts=4 tw=100 */
