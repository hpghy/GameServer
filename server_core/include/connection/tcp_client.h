/***************************************************************************
 *
 * Copyright (c) 2018 hpghy, Inc. All Rights Reserved
 * $Id$
 *
 **************************************************************************/

/**
* @file tcp_client.h
* @author hpghy(790042744@qq.com)
* @date 2018/07/29 14:04:55
* @version $Revision$
* @brief
*
**/
#ifndef TCP_CLIENT_H
#define TCP_CLIENT_H

#include <memory>
#include <string>
#include <cstdint>
#include "connection/tcp_connection.h"
#include "util/timer_manager.h"

// TODO...考虑上层再封装一层，工作在主线程, 并且把重连也挪到上层
class TcpClient: public TcpConnection
{
    public:
        const static uint32_t reconnect_max_interval;
        using TcpClientPtr = std::shared_ptr<TcpClient>;

    public:
        TcpClient(boost::asio::io_service&, const std::string&, uint32_t);

        // 主线程中执行
        void asyncConnect() override;

        // callback在主线程中执行
        void setConnectCallback(std::function<void(TcpClientPtr, bool)> connect_cb)
        {
            connect_cb_ = connect_cb;
        }
        void setDisconnectCallback(std::function<void(TcpClientPtr)> disconnect_cb)
        {
            disconnect_cb_ = disconnect_cb;
        }

        // 和基类不一样，在io线程中执行
        void onConnected() override;
        void onDisconnected() override;

    private:
        void doAsyncConnect();
        void reconnectTimer(const boost::system::error_code& ec);
        void handleConnect(const boost_err&);
        void handleCloseSocket() override;

    protected:
        tcp_addr 		remote_addr_;
        ConnStatus 	    status_ = ConnStatus::STATUS_DISCONNECTED;
        std::function<void(TcpClientPtr, bool)> connect_cb_;
        std::function<void(TcpClientPtr)> disconnect_cb_;
        boost::asio::deadline_timer     reconnect_timer_;
        uint32_t        reconnect_interval_ = 1;
};
using TcpClientPtr = std::shared_ptr<TcpClient>;

#endif  // TCP_CLIENT_H

/* vim: set ts=4 sw=4 sts=4 tw=100 */
