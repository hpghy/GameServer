/***************************************************************************
 *
 * Copyright (c) 2018 hpghy, Inc. All Rights Reserved
 * $Id$
 *
 **************************************************************************/

/**
* @file connection.cpp
* @author hpghy(790042744@qq.com)
* @date 2018/07/29 14:18:34
* @version $Revision$
* @brief
*
**/

#include "connection/connection.h"

#include <iostream>
#include "server/server.h"
#include "service/request_queue.h"

Connection::Connection(boost::asio::io_service& io_service, std::weak_ptr<Server> server)
    : io_service_(io_service),
      server_ptr_(server),
      rcv_strand_(std::make_shared<boost::asio::strand>(io_service)),
      snd_strand_(std::make_shared<boost::asio::strand>(io_service))
{
}

Connection::~Connection()
{
    channel_ptr_.reset();
}

void Connection::startWork()
{
    // startWork必定是在主线程调用的
    rcv_strand_->post(std::bind(&Connection::doStartWork, std::static_pointer_cast<Connection>(shared_from_this())));
}

int32_t Connection::asyncSend(std::shared_ptr<std::string> data)
{
    if (closed_.load())
    {
        WARN_LOG << "socket has been closed";
        return -1;
    }
    snd_strand_->post(std::bind(&Connection::doAsyncSendData,
                                std::static_pointer_cast<Connection>(shared_from_this()),
                                data));
    return 0;
}

// 主线程执行，使用strand保护
void Connection::disconnect()
{
    //这里没有必要像close_socket一样使用automic_flag控制
    snd_strand_->post(std::bind(&Connection::doDisconnect,
                                std::static_pointer_cast<Connection>(shared_from_this())));
}

// io线程中执行
void Connection::doDisconnect()
{
    if (disconnected_)
    {
        return;
    }
    disconnected_ = true;
    closeSocket();
}

bool Connection::closeSocket()
{
    //其实没有必要使用atomic strand+bool就可以了
    bool false_flag = false;
    //compare_exchange_weak: 需要循环才能避免fail spurious
    //导致其他线程一直处于忙等待状态，不适合这里
    if (closed_.compare_exchange_strong(false_flag, true,
                                        std::memory_order_release,
                                        std::memory_order_relaxed))
    {
        onDisconnected();
        snd_strand_->post(std::bind(&Connection::handleCloseSocket,
                                    std::static_pointer_cast<Connection>(shared_from_this())));
        return true;
    }
    return false;
}

void Connection::onConnected()
{
    ServerPtr server = getServer().lock();
    if (!server)
    {
        WARN_LOG << "server_ptr_.lock is nullptr";
        return;
    }
    server->onConnected(shared_from_this());
}

void Connection::notifyDisconnected()
{
    // 通知主线程执行回调
    auto conn = shared_from_this();
    auto callback = [conn](void)
    {
        conn->onDisconnected();
    };
    QueueRequest* request = new QueueCallbackRequest(callback);
    RequestQueueInst::instance().pushRequest(request);
}

void Connection::onDisconnected()
{
    ServerPtr server = getServer().lock();
    if (!server)
    {
        WARN_LOG << "server_ptr_.lock is nullptr";
        return;
    }
    server->onDisconnected(shared_from_this());
}

/* vim: set ts=4 sw=4 sts=4 tw=100 */
