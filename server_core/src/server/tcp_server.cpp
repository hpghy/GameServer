/***************************************************************************
 *
 * Copyright (c) 2018 hpghy, Inc. All Rights Reserved
 * $Id$
 *
 **************************************************************************/

/**
* @file tcp_server.cpp
* @author hpghy(790042744@qq.com)
* @date 2018/07/29 14:11:19
* @version $Revision$
* @brief
*
**/

#ifdef __APPLE__
#define BOOST_LOG_DYN_LINK 1
#endif

#include "server/tcp_server.h"

#include <boost/lexical_cast.hpp>

#include "util/util.h"
#include "connection/tcp_connection.h"
#include "service/request_queue.h"
#include "service/rpc_service.h"
#include "server/mobile_server.h"

TcpServer::TcpServer(boost::asio::io_service& io_service)
    : Server(io_service),
      acceptor_(io_service),
      conns_(0)
{
    INFO_LOG << "Create TcpServer";
}

TcpServer::~TcpServer()
{
    INFO_LOG << "Destroy TcpServer";
}

void TcpServer::bind(const std::string& ip, uint16_t port)
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
        return;
    }

    setOption();

    acceptor_.bind(endpoint, ec);
    if (ec)
    {
        ERROR_LOG << "acceptor bind failed: " << ec.message();
    }
}

void TcpServer::setOption()
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
    // TODO...设置发送和接受缓存大小
}

void TcpServer::listen()
{
    //在nginx、redis、apache里，默认的backlog值是511
    acceptor_.listen(511);
    asyncReceive();
}

void TcpServer::start()
{
    Server::start();
}

// 主线程和io线程都会访问acceptor, 但是不会同时访问
void TcpServer::asyncReceive()
{
    // 这里没有必要使用strand因为io_service只会存在一个handler
    conn_ptr_ = std::make_shared<TcpConnection>(getIoService(), shared_from_this());
    // accept时新产生的socket会继承监听socket的缓冲区大小
    acceptor_.async_accept(conn_ptr_->socket(), std::bind(&TcpServer::handleAccept,
                           std::static_pointer_cast<TcpServer>(shared_from_this()),
                           std::placeholders::_1));
}

// 同时只有一个io线程会执行该函数
void TcpServer::handleAccept(const boost_err& ec)
{
    if (!ec)
    {
        ++ conns_;
        const auto& rmt_addr = conn_ptr_->socket().remote_endpoint();
        conn_ptr_->setRemoteAddr(rmt_addr);
        INFO_LOG << "new connection " << conns_ << " peer: " << rmt_addr << " addr: " << conn_ptr_->getRemoteAddr();

        // avoid tcp_server has been destoried
        auto conn(conn_ptr_);
        auto callback = [conn](void)
        {
            // 在主线程中执行
            conn->onConnected();
            conn->startWork();
        };
        QueueRequest* request = new QueueCallbackRequest(callback);
        RequestQueueInst::instance().pushRequest(request);

        asyncReceive();
    }
    else
    {
        ERROR_LOG << "handle_accept failed " << ec.value() << " " << ec.message();
    }
}

// io线程结束后在主线程执行
void TcpServer::stop()
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
