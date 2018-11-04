/***************************************************************************
 *
 * Copyright (c) 2018 hpghy, Inc. All Rights Reserved
 * $Id$ * **************************************************************************/ /**
 * @file mobile_client.h * @author hpghy(790042744@qq.com) * @date 2018/07/22 11:36:23
 * @version $Revision$
 * @brief
 *
 **/
#ifndef MOBILE_CLIENT_H
#define MOBILE_CLIENT_H

#include <memory>
#include <boost/asio.hpp>
#include <boost/noncopyable.hpp>
#include "util/util.h"
#include "connection/tcp_client.h"
#include "client_service.h"

// 和GateServer相对应, 考虑继承MobileServer
class MobileClient: public std::enable_shared_from_this<MobileClient>, private boost::noncopyable
{
    public:
        const static uint32_t io_thread_num = 2;

    public:
        MobileClient(boost::asio::io_service& io_service, const std::string& ip, uint16_t port);
        ~MobileClient();

        bool start();
        void stop();
        void run();
        void poll();

        void onConnected(ConnectionPtr conn_ptr, bool connected);
        void onDisconnected(ConnectionPtr conn_ptr);

        std::string param;

    private:
        void doTest(const boost_err&);

    private:
        boost::asio::io_service&  io_service_;
        std::string ip_;
        uint16_t port_;
        bool is_running_ = false;
        bool is_shutdown_ = false;
        std::shared_ptr<boost::asio::io_service::work> work_ptr_;
        std::vector<std::thread> io_threads_vec_;

#ifdef USE_KCP
        KcpClientPtr client_;
        using ClientType = KcpClient;
#else
        TcpClientPtr client_;
        using ClientType = TcpClient;
#endif
        ClientServicePtr service_;

};

#endif  // MOBILE_CLIENT_H

/* vim: set ts=4 sw=4 sts=4 tw=100 */
