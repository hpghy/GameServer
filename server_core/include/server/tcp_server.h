/***************************************************************************
 *
 * Copyright (c) 2018 hpghy, Inc. All Rights Reserved
 * $Id$
 *
 **************************************************************************/

/**
* @file tcp_server.h
* @author hpghy(790042744@qq.com)
* @date 2018/07/29 13:45:56
* @version $Revision$
* @brief
*
**/
#ifndef TCP_SERVER_H
#define TCP_SERVER_H

#include <memory>
#include <boost/asio.hpp>
#include "server.h"
#include "../util/util.h"

class TcpConnection;
using TcpConnectionPtr = std::shared_ptr<TcpConnection>;

class TcpServer: public Server
{
    public:
        explicit TcpServer(boost::asio::io_service&);
        ~TcpServer();

        void bind(const std::string& ip, uint16_t port) override;
        void listen() override;
        void start() override;
        void stop() override;

    private:
        void setOption();
        void asyncReceive();
        void handleAccept(const boost_err&);

    private:
        boost::asio::ip::tcp::acceptor acceptor_;
        TcpConnectionPtr         conn_ptr_;
        uint32_t                 conns_;
};
using TcpServerPtr = std::shared_ptr<TcpServer>;

#endif  // TCP_SERVER_H

/* vim: set ts=4 sw=4 sts=4 tw=100 */
