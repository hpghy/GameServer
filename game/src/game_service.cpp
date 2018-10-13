/***************************************************************************
 *
 * Copyright (c) 2018, Inc. All Rights Reserved
 * $Id$
 *
 **************************************************************************/

/**
* @file game_service.cpp
* @author hpghy(790042744@qq.com)
* @date 2018/07/01 21:32:05
* @version $Revision$
* @brief
*
**/

#include "game_service.h"

#include<boost/core/ignore_unused.hpp>
#include "util/util.h"

GameService::GameService()
    : BaseClass()
{
    INFO_LOG << "Create GameService";
}

GameService::~GameService()
{
    INFO_LOG << "Destroy GameService";
}

DEFINE_SERVICE_RPC(GameService, loginRequest, proto::LoginRequest)
{
    auto device_info = request->device_info();
    INFO_LOG << channel_->getRemoteAddr() << " deviceid: " << device_info.deviceid();
    auto stub = getStub();
    if (!stub)
    {
        WARN_LOG << "getStub() is nullptr";
        return;
    }

    proto::LoginReply msg;
    auto* reply_device = msg.mutable_device_info();
    *reply_device = device_info;
    msg.set_retcode(0);
    msg.set_player_data("please serialize player data here");
    stub->loginReply(nullptr, &msg, nullptr, nullptr);
}

DEFINE_SERVICE_RPC(GameService, callEntityRpc, proto::RpcMessage)
{
    INFO_LOG << channel_->getRemoteAddr() << " in call_entity_method " << request->id()
             << " " << request->method() << " parameter: " << request->params();
    auto stub = getStub();
    if (!stub)
    {
        WARN_LOG << "getStub() is nullptr";
        return;
    }
    proto::RpcMessage msg;
    msg.set_method("rpc_c_from_gameserver");
    msg.set_id(request->id());
    msg.set_params(request->params());
    auto* device_info_ptr = msg.mutable_device_info();
    *device_info_ptr = request->device_info();
    stub->callEntityRpc(nullptr, &msg, nullptr, nullptr);
}

/* vim: set ts=4 sw=4 sts=4 tw=100 */
