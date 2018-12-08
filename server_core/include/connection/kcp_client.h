/***************************************************************************
 *
 * Copyright (c) 2018 hpghy, Inc. All Rights Reserved
 * $Id$
 *
 **************************************************************************/

/**
* @file kcp_client.h
* @author hpghy(790042744@qq.com)
* @date 2018/11/17 21:11:22
* @version $Revision$
* @brief
*
**/
#ifndef KCP_CLIENT_H
#define KCP_CLIENT_H

#include <boost/asio.hpp>
#include <boost/bind.hpp>
#include "connection/kcp_connection.h"


class MobileClient;
class KcpClient: public KcpConnection
{
    public:
        const static uint32_t connect_timeout = 10;
        const static uint32_t reconnect_max_interval;
        using KcpClientPtr = std::shared_ptr<KcpClient>;

    public:
        KcpClient(boost::asio::io_service&, const std::string&, uint32_t);

        // 主线程中执行
        void asyncConnect() override;

        // callback在主线程中执行
        void setConnectCallback(std::function<void(KcpClientPtr, bool)> connect_cb)
        {
            connect_cb_ = connect_cb;
        }
        void setDisconnectCallback(std::function<void(KcpClientPtr)> disconnect_cb)
        {
            disconnect_cb_ = disconnect_cb;
        }

        void onConnected(bool is_connected);
        // 和基类不一样，在io线程中执行
        void onConnected() override;
        void onDisconnected() override;

    private:
        void doAsyncConnect();
        void doConnect();
        bool directSendData(const char* data, std::size_t bytes);
        void asyncReceiveSync();
        void handleReceiveSync(const boost_err& ec, std::size_t bytes);
        void handleConnect(const boost_err&);
        void handleCloseSocket() override;

    private:
        udp_addr 		remote_addr_;
        ConnStatus 	    status_ = ConnStatus::STATUS_DISCONNECTED;
        std::function<void(KcpClientPtr, bool)> connect_cb_;
        std::function<void(KcpClientPtr)> disconnect_cb_;
        boost::asio::deadline_timer     reconnect_timer_;
        uint32_t        reconnect_interval_ = 1;
};
using KcpClientPtr = std::shared_ptr<KcpClient>;

#endif  // KCP_CLIENT_H

/* vim: set ts=4 sw=4 sts=4 tw=100 */
