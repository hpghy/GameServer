/***************************************************************************
 *
 * Copyright (c) 2018 hpghy, Inc. All Rights Reserved
 * $Id$
 *
 **************************************************************************/

/**
* @file mobile_client.cpp
* @author hpghy(790042744@qq.com)
* @date 2018/07/22 11:45:29
* @version $Revision$
* @brief
*
**/

#include "mobile_client.h"

#include <boost/uuid/uuid.hpp>
#include <boost/uuid/uuid_io.hpp>
#include <boost/uuid/uuid_generators.hpp>
#include <boost/format.hpp>
#include "util/timer_manager.h"
#include "util/util.h"
#include "util/timer_manager.h"
#include "connection/rpc_channel.h"
#include "service/request_queue.h"

#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wdeprecated-declarations"
#include "proto/client_gate.pb.h"
#pragma clang diagnostic pop

MobileClient::MobileClient(boost::asio::io_service& io_service,
                           const std::string& ip,
                           uint16_t port)
    : io_service_(io_service),
      ip_(ip),
      port_(port)
{
    client_ptr_ = std::make_shared<ClientType>(io_service, ip, port);
    service_ = std::make_shared<ClientService>();
    INFO_LOG << "Create MobileClient";
}

MobileClient::~MobileClient()
{
    INFO_LOG << "Destory MobileClient";
}

bool MobileClient::start()
{
    if (is_running_)
    {
        return false;
    }
    is_running_ = true;

    // 客户端不处理信号
    client_ptr_->setConnectCallback(bind(&MobileClient::onConnected,
                                         std::static_pointer_cast<MobileClient>(shared_from_this()),
                                         std::placeholders::_1,
                                         std::placeholders::_2));
    client_ptr_->setDisconnectCallback(bind(&MobileClient::onDisconnected,
                                            std::static_pointer_cast<MobileClient>(shared_from_this()),
                                            std::placeholders::_1));
    client_ptr_->asyncConnect();

    work_ptr_ = std::make_shared<boost::asio::io_service::work>(io_service_);
    auto self(shared_from_this());
    for (uint32_t idx = 0; idx < io_thread_num; ++idx)
    {
        io_threads_vec_.emplace_back([self]() {self->io_service_.run();});
    }
    return true;
}

void MobileClient::stop()
{
    INFO_LOG << "Stop!!!";
    is_shutdown_ = true;
}

void MobileClient::run()
{
    while (true)
    {
        if (is_shutdown_)
        {
            INFO_LOG << "is_shutdown_ is true, break run";
            break;
        }
        poll();
    }

    // stop service, 使得io线程函数结束
    work_ptr_.reset();
    io_service_.stop();

    // 等待io线程的正常结束
    for (auto& thread : io_threads_vec_)
    {
        thread.join();
    }

    service_.reset();
}

void MobileClient::poll()
{
    auto start = stamp_microsecond();
    //处理rpc
    auto cnt = RequestQueueInst::instance().processRequest(100);
    if (cnt > 50)
    {
        INFO_LOG << "processRequest " << cnt;
    }
    // 处理定时器
    TimerManagerInst::instance().tick(50);

    auto delta = stamp_microsecond() - start;
    if (delta < 1000)
    {
        std::this_thread::sleep_for(std::chrono::microseconds(5000));
    }
}

void MobileClient::onConnected(ConnectionPtr conn, bool connected)
{
    if (!conn || !connected)
    {
        stop();
        return;
    }

    // 构造conn <--> channel <--> service 关系
    auto channel = client_ptr_->getChannel().lock();
    if (!channel)
    {
        channel = std::make_shared<RpcChannel>(client_ptr_);
        client_ptr_->setChannel(channel);

        service_->setChannel(channel);
        service_->setParam(this->param);
        channel->setService(service_);
    }

    // 测试30s就关闭
    TimerManagerInst::instance().addTimer(30, shared_from_this(), &MobileClient::stop);

    // 发送LoginRequest
    if (service_)
    {
        proto::LoginRequest msg;
        msg.set_login_type(0);
        auto uuid = boost::uuids::random_generator()();
        std::string deviceid = boost::uuids::to_string(uuid);
        INFO_LOG << "deviceid: " << deviceid;

        auto* device_info = msg.mutable_device_info();
        device_info->set_deviceid(deviceid);
        service_->getStub()->loginRequest(nullptr, &msg, nullptr, nullptr);
        DEBUG_LOG << "HPTEST loginRequest over";
    }
}

void MobileClient::onDisconnected(ConnectionPtr)
{
    INFO_LOG << "onDisconnected";
    stop();
}


/* vim: set ts=4 sw=4 sts=4 tw=100 */
