/***************************************************************************
 *
 * Copyright (c) 2018 hpghy, Inc. All Rights Reserved
 * $Id$
 *
 **************************************************************************/

/**
* @file game_client_service.h
* @author hpghy(790042744@qq.com)
* @date 2018/09/08 19:17:42
* @version $Revision$
* @brief
*
**/
#ifndef GAME_CLIENT_SERVICE_H
#define GAME_CLIENT_SERVICE_H

#include "service/rpc_service.h"

#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wdeprecated-declarations"
#include "proto/gate_game.pb.h"
#pragma clang diagnostic pop
using namespace std;

using ClientServiceClass = RpcService<proto::IGameClientService, proto::IGameService_Stub>;
class GameClientService : public ClientServiceClass
{
    public:
        DECLARE_SERVICE_RPC(loginReply, proto::LoginReply);
        DECLARE_SERVICE_RPC(callEntityRpc, proto::RpcMessage);
};

class GameClientServiceFactory : public ServiceFactory
{
    public:
        IServicePtr createService() override;
};

#endif  // GAME_CLIENT_SERVICE_H

/* vim: set ts=4 sw=4 sts=4 tw=100 */
