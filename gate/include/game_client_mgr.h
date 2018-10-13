/***************************************************************************
 *
 * Copyright (c) 2018 hpghy, Inc. All Rights Reserved
 * $Id$
 *
 **************************************************************************/

/** * @file game_client_mgr.h
* @author hpghy(790042744@qq.com)
* @date 2018/09/08 21:44:38
* @version $Revision$
* @brief
*
**/
#ifndef GAME_CLIENT_MGR_H
#define GAME_CLIENT_MGR_H

#include <unordered_map>
#include <memory>
#include <vector>

#include "gate_config.h"
#include "game_client_service.h"
#include "service/service_manager.h"
#include "connection/tcp_client.h"

class MobileServer;
using MobileServerPtr = std::shared_ptr<MobileServer>;

// TODO...后续抽取出基类
class GameClientMgr : public std::enable_shared_from_this<GameClientMgr>
{
    public:
        GameClientMgr(ServiceFactoryPtr factory);
        ~GameClientMgr();

        void setMobileServer(MobileServerPtr container);

    public:
        // 添加一个GameServer配置
        void addGameConfig(const vector<ServerConfig>& config_vec);
        void addGameConfig(const ServerConfig& config);
        void createGameClient(const ServerConfig& config);
        void start();
        void stop();

        std::string getRandomGameName();
        GameClientService::StubPtr getGameStub(const std::string& name);

    private:
        MobileServerPtr container_;
        std::unordered_map<std::string, TcpClientPtr> client_map_;
        std::unordered_map<std::string, ServerConfig> game_config_map_;
        std::unordered_set<std::string> available_games_;
        ServiceManager service_mgr_;
        bool is_running_ = false;
};
using GameClientMgrPtr = std::shared_ptr<GameClientMgr>;

#endif  // GAME_CLIENT_MGR_H

/* vim: set ts=4 sw=4 sts=4 tw=100 */
