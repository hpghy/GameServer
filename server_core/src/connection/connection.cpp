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

Connection::Connection(boost::asio::io_service& io_service, std::weak_ptr<Server> server_wptr)
    : io_service_(io_service),
      server_wptr_(server_wptr),
      rcv_strand_(std::make_shared<boost::asio::strand>(io_service)),
      snd_strand_(std::make_shared<boost::asio::strand>(io_service))
{
}

Connection::~Connection()
{
    channel_wptr_.reset();
}

void Connection::startWork()
{
    // startWork必定是在主线程调用的
    auto self = shared_from_this();
    rcv_strand_->post([self]() { self->doStartWork(); });
}

int32_t Connection::asyncSend(std::shared_ptr<std::string> data)
{
    if (closed_.load())
    {
        WARN_LOG << "socket has been closed";
        return -1;
    }
    auto self = shared_from_this();
    snd_strand_->post([self, data]() { self->doAsyncSendData(data); });
    return 0;
}

// 主线程执行，使用strand保护
void Connection::disconnect()
{
    //这里没有必要像close_socket一样使用automic_flag控制
    auto self = shared_from_this();
    snd_strand_->post([self]() { self->doDisconnect();  });
}

// io线程中执行
void Connection::doDisconnect()
{
    if (is_disconnected_)
    {
        return;
    }
    is_disconnected_ = true;
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
        // 需要保证在IO线程执行handleCloseSocket
        dispatchCloseSocket();
        return true;
    }
    return false;
}

void Connection::dispatchCloseSocket()
{
    auto self = shared_from_this();
    snd_strand_->post([self]() { self->handleCloseSocket(); });
}

void Connection::onConnected()
{
    ServerPtr server_ptr = getServer().lock();
    if (!server_ptr)
    {
        WARN_LOG << "server_ptr_.lock is nullptr";
        return;
    }
    server_ptr->onConnected(shared_from_this());
}

void Connection::notifyDisconnected()
{
    // 通知主线程执行回调
    auto conn_ptr = shared_from_this();
    auto callback = [conn_ptr](void)
    {
        conn_ptr->onDisconnected();
    };
    QueueRequest* request = new QueueCallbackRequest(callback);
    RequestQueueInst::instance().pushRequest(request);
}

void Connection::onDisconnected()
{
    ServerPtr server_ptr = getServer().lock();
    if (!server_ptr)
    {
        WARN_LOG << "server_ptr_.lock is nullptr";
        return;
    }
    server_ptr->onDisconnected(shared_from_this());
}

/* vim: set ts=4 sw=4 sts=4 tw=100 */
