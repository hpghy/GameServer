/***************************************************************************
 *
 * Copyright (c) 2018 hpghy, Inc. All Rights Reserved
 * $Id$
 *
 **************************************************************************/

/**
* @file mobile_server.cpp
* @author hpghy(790042744@qq.com)
* @date 2018/07/29 14:08:54
* @version $Revision$
* @brief
*
**/

#include "server/mobile_server.h"

#include "util/util.h"
#include "util/timer_manager.h"
#include "service/request_queue.h"
#include "http/http_handle_manager.h"

MobileServer::MobileServer(ServiceFactoryPtr factory): io_service_(), service_mgr_(factory)
{}

MobileServer::~MobileServer()
{}

bool MobileServer::start()
{
    if (is_running_)
    {
        return false;
    }
    is_running_ = true;

    initBeforeStartServer();
    startServer();
    initBeforeStartThread();

    work_ = std::make_shared<boost::asio::io_service::work>(io_service_);
    auto self(shared_from_this());
    for (uint32_t idx = 0; idx < io_thread_num; ++ idx)
    {
        io_threads_.emplace_back([self]()
        {
            self->io_service_.run();
        });
    }
    return true;
}

void MobileServer::initBeforeStartServer()
{}

void MobileServer::startServer()
{}

void MobileServer::initBeforeStartThread()
{}

void MobileServer::stop()
{
    is_shutdown_ = true;
}

void MobileServer::run()
{
    while (true)
    {
        if (is_shutdown_)
        {
            INFO_LOG << "break run loop because of shutdown";
            break;
        }
        poll();
    }

    // 使得io线程函数正常结束
    work_.reset();
    io_service_.stop();

    INFO_LOG << "wait children thread exit";
    for (size_t idx = 0; idx < io_threads_.size(); ++idx)
    {
        io_threads_[idx].join();
    }

    INFO_LOG << "all children thread exit";
    stopServer();

    INFO_LOG << "exit from run()";
}

void MobileServer::stopServer()
{}

void MobileServer::poll()
{
    auto start = stamp_microsecond();

    if (is_reload_config_)
    {
        onReloadConfig();
        is_reload_config_ = false;
    }

    //处理rpc
    auto cnt = RequestQueueInst::instance().processRequest(100);
    if (cnt > 50)
    {
        INFO_LOG << "processRequest " << cnt;
    }

    // 处理http请求
    HttpHandleManagerInst::instance().processRequest(50);

    // 处理定时器
    TimerManagerInst::instance().tick(50);

    // 判断队列的长度 --> 睡眠一定的时间
    auto delta = stamp_microsecond() - start;
    if (delta < 1000)
    {
        std::this_thread::sleep_for(std::chrono::microseconds(5000));
    }
}

void MobileServer::onConnected(ConnectionPtr conn)
{
    DEBUG_LOG << "MobileServer::onConnected";
    service_mgr_.onConnected(conn);
}

void MobileServer::onDisconnected(ConnectionPtr conn)
{
    service_mgr_.onDisconnected(conn);
}

void MobileServer::onReloadConfig()
{}


/* vim: set ts=4 sw=4 sts=4 tw=100 */
