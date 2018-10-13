/***************************************************************************
 *
 * Copyright (c) 2018 hpghy, Inc. All Rights Reserved
 * $Id$
 *
 **************************************************************************/

/**
* @file http_server.cpp
* @author hpghy(790042744@qq.com)
* @date 2018/07/29 14:09:46
* @version $Revision$
* @brief
*
**/

#ifdef __APPLE__
#define BOOST_LOG_DYN_LINK 1
#endif

#include <boost/lexical_cast.hpp>
#include "server/http_server.h"
#include "util/util.h"
#include "http/http_connection.h"

HttpServer::HttpServer(boost::asio::io_service& io_service)
    : Server(io_service),
      acceptor_(io_service)
{
    DEBUG_LOG << "Create HttpServer";
}

HttpServer::~HttpServer()
{
    DEBUG_LOG << "Destroy HttpServer";
}

void HttpServer::bind(const std::string& ip, uint16_t port)
{
    Server::bind(ip, port);

    boost::asio::ip::tcp::resolver resolver(getIoService());
    boost::asio::ip::tcp::resolver::query query(getHost(), boost::lexical_cast<std::string>(getPort()));
    boost::asio::ip::tcp::endpoint endpoint = *resolver.resolve(query);

    if (acceptor_.is_open())
        acceptor_.close();

    boost_err ec;
    acceptor_.open(endpoint.protocol(), ec);
    if (ec)
    {
        ERROR_LOG << "acceptor open failed: " << ec.message();
    }

    setOption();

    acceptor_.bind(endpoint, ec);
    if (ec)
    {
        ERROR_LOG << "acceptor bind failed: " << ec.message();
    }
}

void HttpServer::setOption()
{
    boost::asio::socket_base::reuse_address option(true);
    acceptor_.set_option(option);
    acceptor_.set_option(boost::asio::ip::tcp::socket::keep_alive(true));
    acceptor_.set_option(boost::asio::ip::tcp::no_delay(true));
    boost::system::error_code ec;
    acceptor_.non_blocking(true, ec);
    if (ec)
    {
        ERROR_LOG << "non_blocking failed: " << ec.message();
    }
}

void HttpServer::listen()
{
    acceptor_.listen(511);
    asyncReceive();
}

void HttpServer::start()
{
    Server::start();
}

void HttpServer::asyncReceive()
{
    conn_ptr_ = std::make_shared<HttpConnection>(getIoService());
    acceptor_.async_accept(conn_ptr_->socket(), std::bind(&HttpServer::handleAccept,
                           std::static_pointer_cast<HttpServer>(shared_from_this()),
                           std::placeholders::_1));

}

void HttpServer::handleAccept(const boost_err& ec)
{
    if (!ec)
    {
        INFO_LOG << "new connection " << conn_ptr_->socket().remote_endpoint();
        // 这里不调用onConnected
        // TODO...startWork只能在主线程调用
        conn_ptr_->startWork();
        asyncReceive();
    }
    else
    {
        ERROR_LOG << "handle_accept failed " << ec.value() << " " << ec.message();
    }
}

void HttpServer::stop()
{
    Server::stop();
    if (acceptor_.is_open())
    {
        boost_err ec;
        acceptor_.close(ec);
        if (ec)
        {
            ERROR_LOG << "TcpServer close socket error " << ec.value()
                      << " " << ec.message();
        }
    }
}

/* vim: set ts=4 sw=4 sts=4 tw=100 */
