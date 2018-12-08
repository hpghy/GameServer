/**************************************************************************
 *
 * Copyright (c) 2018 hpghy, Inc. All Rights Reserved
 * $Id$
 *
 **************************************************************************/

/**
* @file gate_server.cpp
* @author hpghy(790042744@qq.com)
* @date 2018/07/01 21:44:05
* @version $Revision$
* @brief
*
**/

#include "gate_server.h"

#include "server/tcp_server.h"
#include "server/kcp_server.h"
#include "server/http_server.h"
#include "service/service_manager.h"
#include "util/timer_manager.h"
#include "http/http_handle_manager.h"
#include "gate_service.h"

GateServer::GateServer(const GateConfig& gate_config, const ServerConfig& self_config)
    : MobileServer(std::make_shared<GateServiceFactory>()),
      config_(gate_config),
      self_config_(self_config)
{
    if (config_.kcp_enabled)
    {
        kcp_server_ptr_ = make_shared<KcpServer>(getIoService());
    }
    if (self_config_.http_port > 0)
    {
        http_server_ptr_ = make_shared<HttpServer>(getIoService());
    }
    tcp_server_ptr_ = std::make_shared<TcpServer>(getIoService());
    game_client_mgr_ptr_ = std::make_shared<GameClientMgr>(make_shared<GameClientServiceFactory>());
    INFO_LOG << "Create GateServer: " << self_config_.name << " Succ!";
}

void GateServer::startServer()
{
    auto self = shared_from_this();
    if (tcp_server_ptr_)
    {
        // TODO...抽取一个init函数
        tcp_server_ptr_->setMobileServer(self);
        tcp_server_ptr_->bind(self_config_.host, self_config_.port);
        tcp_server_ptr_->listen();
        tcp_server_ptr_->start();
    }
    if (kcp_server_ptr_)
    {
        kcp_server_ptr_->setMobileServer(self);
        kcp_server_ptr_->bind(self_config_.host, self_config_.port);
        kcp_server_ptr_->listen();
        kcp_server_ptr_->start();
    }
    if (http_server_ptr_)
    {
        http_server_ptr_->setMobileServer(self);
        http_server_ptr_->bind(self_config_.http_host, self_config_.http_port);
        http_server_ptr_->listen();
        http_server_ptr_->start();
    }

    for (const auto& pair : config_.server_configs)
    {
        if (0 == pair.first.find("game"))
        {
            game_client_mgr_ptr_->addGameConfig(pair.second);
        }
    }
    game_client_mgr_ptr_->setMobileServer(shared_from_this());
    game_client_mgr_ptr_->start();
}

void GateServer::stopServer()
{
    if (tcp_server_ptr_)
    {
        tcp_server_ptr_->stop();
    }
    if (tcp_server_ptr_)
    {
        tcp_server_ptr_->stop();
    }
    if (http_server_ptr_)
    {
        http_server_ptr_->stop();
    }

    game_client_mgr_ptr_->stop();
}

void GateServer::onDisconnected(ConnectionPtr conn)
{
    if (!conn)
    {
        return;
    }
    MobileServer::onDisconnected(conn);

    // 需要移除DeviceClient
    auto channel_ptr = conn->getChannel();
    if (!channel_ptr)
    {
        WARN_LOG << "channel_wptr.lock return nullptr";
        return;
    }
    auto service_wptr = channel_ptr->getService();
    auto service_ptr = service_wptr.lock();
    if (!service_ptr)
    {
        WARN_LOG << "service_wptr.lock return nullptr";
        return;
    }
    auto gate_service = std::dynamic_pointer_cast<GateService>(service_ptr);
    if (!gate_service)
    {
        WARN_LOG << "dynamic_pointer_cast GateService failed";
        return;
    }
    delDeviceClient(gate_service->getDeviceId());
}

GateServer::GameStubPtr GateServer::getGameStubByDeviceId(const std::string& deviceid)
{
    auto iter = device_client_infos_map_.find(deviceid);
    if (iter == device_client_infos_map_.end())
    {
        WARN_LOG << "find DeviceInfo deviceid: " << deviceid << " failed";
        return GameStubPtr();
    }
    return game_client_mgr_ptr_->getGameStub(iter->second.game_name);
}

GateServer::ClientStubPtr GateServer::getClientStubByDeviceId(const std::string& deviceid)
{
    auto iter = device_client_infos_map_.find(deviceid);
    if (iter == device_client_infos_map_.end())
    {
        WARN_LOG << "find DeviceInfo deviceid: " << deviceid << " failed";
        return ClientStubPtr();
    }
    const auto& rmt_addr = iter->second.addr;
    IServicePtr service = getService(rmt_addr);
    if (!service)
    {
        WARN_LOG << "find Service rmt_addr: " << rmt_addr << " failed";
        return ClientStubPtr();
    }
    std::shared_ptr<GateService> gate_service = std::dynamic_pointer_cast<GateService>(service);
    if (!gate_service)
    {
        WARN_LOG << "dynamic_pointer_cast GateService for " << deviceid << " failed";
        return ClientStubPtr();
    }
    //DEBUG_LOG << "deviceId: " << deviceid << " rmt_addr: " << rmt_addr;
    return gate_service->getStub();
}

void GateServer::addDeviceClient(const DeviceClient& device_client)
{
    if (device_client.game_name.empty())
    {
        WARN_LOG << "deviceid: " << device_client.deviceid << " game_name is empty";
        return;
    }
    auto iter = device_client_infos_map_.find(device_client.deviceid);
    if (iter != device_client_infos_map_.end())
    {
        WARN_LOG << "duplicate deviceid: " << device_client.deviceid << " and overwrite it";
    }
    device_client_infos_map_[device_client.deviceid] = device_client;
}

void GateServer::delDeviceClient(const std::string& deviceid)
{
    device_client_infos_map_.erase(deviceid);
}

IServicePtr GateServiceFactory::createService()
{
    return std::make_shared<GateService>();
}


/* vim: set ts=4 sw=4 sts=4 tw=100 */
