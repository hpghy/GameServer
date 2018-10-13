/***************************************************************************
 *
 * Copyright (c) 2018, Inc. All Rights Reserved
 * $Id$
 *
 **************************************************************************/

/**
* @file game_service.h
* @author hpghy(790042744@qq.com)
* @date 2018/07/01 21:18:48
* @version $Revision$
* @brief
*
**/
#ifndef GAME_SERVICE_H
#define GAME_SERVICE_H

#include "service/rpc_service.h"

#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wdeprecated-declarations"
#include "proto/gate_game.pb.h"
#pragma clang diagnostic pop

using BaseClass = RpcService<proto::IGameService, proto::IGameClientService_Stub>;
class GameService : public BaseClass
{
    public:
        GameService();
        ~GameService();

        DECLARE_SERVICE_RPC(loginRequest, proto::LoginRequest)
        DECLARE_SERVICE_RPC(callEntityRpc, proto::RpcMessage)
};

class GameServiceFactory : public ServiceFactory
{
    public:
        virtual IServicePtr createService()
        {
            return std::static_pointer_cast<IService>(std::make_shared<GameService>());
        }
};

#endif  // GAME_SERVICE_H

/* vim: set ts=4 sw=4 sts=4 tw=100 */
