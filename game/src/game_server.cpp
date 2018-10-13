/***************************************************************************
 *
 * Copyright (c) 2018, Inc. All Rights Reserved
 * $Id$
 *
 **************************************************************************/

/**
* @file game_server.cpp
* @author hpghy(790042744@qq.com)
* @date 2018/07/01 21:25:49
* @version $Revision$
* @brief
*
**/

#include "game_server.h"
#include "game_service.h"
#include "util/util.h"
#include "server/tcp_server.h"
#include "server/http_server.h"

GameServer::GameServer(const GameConfig& game_config, const ServerConfig& self_config)
    : MobileServer(make_shared<GameServiceFactory>()),
      config_(game_config),
      self_config_(self_config),
      tcp_server_(make_shared<TcpServer>(getIoService()))
{
    if (self_config_.http_port > 0)
    {
        http_server_ = make_shared<HttpServer>(getIoService());
    }
    INFO_LOG << "Create GameServer";
}

GameServer::~GameServer()
{
    INFO_LOG << "Destroy GameServer";
}

void GameServer::startServer()
{
    auto self = shared_from_this();
    if (tcp_server_)
    {
        tcp_server_->setMobileServer(self);
        tcp_server_->bind(self_config_.host, self_config_.port);
        tcp_server_->listen();
        tcp_server_->start();
    }
    if (http_server_)
    {
        http_server_->setMobileServer(self);
        http_server_->bind(self_config_.http_host, self_config_.http_port);
        http_server_->listen();
        http_server_->start();
    }
}

void GameServer::stopServer()
{
    if (tcp_server_)
    {
        tcp_server_->stop();
    }
    if (http_server_)
    {
        http_server_->stop();
    }
}

void GameServer::onReloadConfig()
{
    // TODO...加载数值表
}

/* vim: set ts=4 sw=4 sts=4 tw=100 */
