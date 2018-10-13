/***************************************************************************
 *
 * Copyright (c) 2018 hpghy, Inc. All Rights Reserved
 * $Id$
 *
 **************************************************************************/

/**
* @file http_server.h
* @author hpghy(790042744@qq.com)
* @date 2018/07/29 13:42:10
* @version $Revision$
* @brief
*
**/
#ifndef HTTP_SERVER_H
#define HTTP_SERVER_H

#include "server/server.h"
#include "util/util.h"

class HttpConnection;
using HttpConnectionPtr = std::shared_ptr<HttpConnection>;

class HttpServer: public Server
{
    public:
        HttpServer(boost::asio::io_service&);
        ~HttpServer();

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
        HttpConnectionPtr           conn_ptr_;
};
using HttpServerPtr = std::shared_ptr<HttpServer>;

#endif  // HTTP_SERVER_H

/* vim: set ts=4 sw=4 sts=4 tw=100 */
