/***************************************************************************
 *
 * Copyright (c) 2018 hpghy, Inc. All Rights Reserved
 * $Id$
 *
 **************************************************************************/

/**
* @file iclient.h
* @author hpghy(790042744@qq.com)
* @date 2018/11/17 21:58:00
* @version $Revision$
* @brief
*
**/
#ifndef ICLIENT_H
#define ICLIENT_H

class IClient
{
        /*
        public:
            enum class Status
            {
                STATUS_DISCONNECTED = 0,
                STATUS_CONNECTING = 1,
                STATUS_CONNECTED = 2,
            };
            const static uint32_t reconnect_max_interval;

            // 主线程或是io线程
            virtual void asyncConnect();

            // callback在主线程中执行
            void setConnectCallback(std::function<void(KcpClientPtr, bool)> connect_cb)
            {
                connect_cb_ = connect_cb;
            }
            void setDisconnectCallback(std::function<void(KcpClientPtr)> disconnect_cb)
            {
                disconnect_cb_ = disconnect_cb;
            }

            // 和基类不一样，在io线程中执行
            void onConnected() override;
            void onDisconnected() override;

        private:
            void reconnectTimer(const boost::system::error_code& ec);
            void handleConnect(const boost_err& ec);

        private:
            Status 	        status_ = Status::STATUS_DISCONNECTED;
            std::function<void(KcpClientPtr, bool)> connect_cb_;
            std::function<void(KcpClientPtr)> disconnect_cb_;
            boost::asio::deadline_timer     reconnect_timer_;
            uint32_t        reconnect_interval_ = 1;
        */
};

#endif  // ICLIENT_H

/* vim: set ts=4 sw=4 sts=4 tw=100 */
