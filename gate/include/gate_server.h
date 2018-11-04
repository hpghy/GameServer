/***************************************************************************
 *
 * Copyright (c) 2018, Inc. All Rights Reserved
 * $Id$
 *
 **************************************************************************/

/**
* @file gate_server.h
* @author hpghy(790042744@qq.com)
* @date 2018/07/01 21:39:57
* @version $Revision$
* @brief
*
**/
#ifndef GATE_SERVER_H
#define GATE_SERVER_H

#include <memory>
#include "server/mobile_server.h"
#include "util/util.h"
#include "gate_config.h"
#include "game_client_mgr.h"
#include "gate_service.h"

class TcpServer;
using TcpServerPtr = std::shared_ptr<TcpServer>;
class KcpServer;
using KcpServerPtr = std::shared_ptr<KcpServer>;
class HttpServer;
using HttpServerPtr = std::shared_ptr<HttpServer>;

class GateServer: public MobileServer
{
    public:
        const static uint32_t io_thread_num = 3;
        using GameStubPtr = GameClientService::StubPtr;
        using ClientStubPtr = GateService::StubPtr;

    public:
        GateServer(const GateConfig& gate_config, const ServerConfig& self_config);
        ~GateServer() = default;

        void startServer() override;
        void stopServer() override;
        void onDisconnected(ConnectionPtr conn) override;

        std::string getRandomGameName() { return game_client_mgr_ptr_->getRandomGameName(); }
        GameStubPtr getGameStubByDeviceId(const std::string& deviceid);
        ClientStubPtr getClientStubByDeviceId(const std::string& deviceid);

        void addDeviceClient(const DeviceClient& device_client);
        void delDeviceClient(const std::string& deviceid);

    private:
        GateConfig config_;
        ServerConfig self_config_;

        // 对client提供服务
        TcpServerPtr tcp_server_ptr_;
        KcpServerPtr kcp_server_ptr_;
        HttpServerPtr http_server_ptr_;

        GameClientMgrPtr game_client_mgr_ptr_;      // 连接所有的gameserver

        // 需要记录客户端和game_client的映射关系
        std::unordered_map<std::string, DeviceClient> device_client_infos_map_;
};

#endif  // GATE_SERVER_H

/* vim: set ts=4 sw=4 sts=4 tw=100 */
