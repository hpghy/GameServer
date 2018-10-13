/***************************************************************************
 * * Copyright (c) 2018 hpghy, Inc. All Rights Reserved
 * $Id$
 *
 **************************************************************************/

/**
* @file game_client_mgr.cpp
* @author hpghy(790042744@qq.com)
* @date 2018/09/08 22:04:05
* @version $Revision$
* @brief
*
**/

#include "game_client_mgr.h"

#include <random>
#include "game_client_service.h"
#include "util/util.h"
#include "server/mobile_server.h"

GameClientMgr::GameClientMgr(ServiceFactoryPtr factory)
    : service_mgr_(factory)
{
    DEBUG_LOG << "create GameClientMgr";
}

GameClientMgr::~GameClientMgr()
{
    DEBUG_LOG << "destory GameClientMgr";
}

void GameClientMgr::setMobileServer(MobileServerPtr container)
{
    if (!container)
    {
        return;
    }
    container_ = container;
}

void GameClientMgr::addGameConfig(const vector<ServerConfig>& config_vec)
{
    for (const auto& config : config_vec)
    {
        addGameConfig(config);
    }
}

// 添加一个GameServer配置
void GameClientMgr::addGameConfig(const ServerConfig& config)
{
    if (!game_config_map_.insert(std::make_pair(config.name, config)).second)
    {
        WARN_LOG << "addGameConfig " << config.name << " duplicated";
        return;
    }
    if (is_running_)
    {
        createGameClient(config);
    }
}

void GameClientMgr::createGameClient(const ServerConfig& config)
{
    auto game_client = std::make_shared<TcpClient>(container_->getIoService(), config.host, config.port);
    if (!client_map_.insert(std::make_pair(config.name, game_client)).second)
    {
        WARN_LOG << "insert gameclient: " << config.name << " failed";
        return;
    }

    auto self = std::static_pointer_cast<GameClientMgr>(shared_from_this());
    auto callback = [self, config](TcpClientPtr conn, bool connected)
    {
        if (!conn || !connected)
        {
            return;
        }
        self->service_mgr_.onConnected(conn);
        self->available_games_.emplace(config.name);
        DEBUG_LOG << "onConnected: " << config.name << " rmt_addr: " << conn->getRemoteAddr();
    };
    game_client->setConnectCallback(callback);
    game_client->setDisconnectCallback([self, config](TcpClientPtr conn)
    {
        self->available_games_.erase(config.name);
        self->service_mgr_.onDisconnected(conn);
        // TODO...tryConnect
    });
    game_client->asyncConnect();
}

void GameClientMgr::start()
{
    if (is_running_)
    {
        return;
    }
    if (!container_)
    {
        WARN_LOG << "container is null";
        return;
    }
    is_running_ = true;
    for (const auto& pair : game_config_map_)
    {
        createGameClient(pair.second);
    }
}

void GameClientMgr::stop()
{
    for (auto& pair : client_map_)
    {
        pair.second->disconnect();
    }
}

std::string GameClientMgr::getRandomGameName()
{
    if (available_games_.empty())
    {
        return "";
    }
    std::random_device rd;
    size_t idx = rd() % available_games_.size();
    auto iter = available_games_.begin();
    std::advance(iter, idx);
    return *iter;
}

GameClientService::StubPtr GameClientMgr::getGameStub(const std::string& name)
{
    auto iter = client_map_.find(name);
    if (iter == client_map_.end())
    {
        WARN_LOG << "not found " << name << " in client_map_";
        return GameClientService::StubPtr();
    }

    ConnectionPtr conn = std::static_pointer_cast<Connection>(iter->second);
    auto service = service_mgr_.getService(iter->second->getRemoteAddr());
    if (!service)
    {
        WARN_LOG << "not found " << name << " " << iter->second->getRemoteAddr() << " in service_mgr";
        return GameClientService::StubPtr();
    }
    auto game_client_service = std::dynamic_pointer_cast<GameClientService>(service);
    if (!game_client_service)
    {
        WARN_LOG << "IService static_pointer_cast to GameClientService failed";
        return GameClientService::StubPtr();
    }
    return game_client_service->getStub();
}

/* vim: set ts=4 sw=4 sts=4 tw=100 */
