/***************************************************************************
 *
 * Copyright (c) 2018 hpghy, Inc. All Rights Reserved
 * $Id$
 *
 **************************************************************************/

/**
* @file game_client_service.cpp
* @author hpghy(790042744@qq.com)
* @date 2018/09/08 19:22:39
* @version $Revision$
* @brief
*
**/

#include "game_client_service.h"

#include "global.h"
#include "gate_server.h"
#include "gate_service.h"

DEFINE_SERVICE_RPC(GameClientService, loginReply, proto::LoginReply)
{
    const auto& device_info = request->device_info();
    const std::string& deviceid = device_info.deviceid();

    INFO_LOG << "loginReply deviceid: " << deviceid << " retcode: " << request->retcode();

    auto stub_ptr = Global::gate_server->getClientStubByDeviceId(deviceid);
    if (!stub_ptr)
    {
        WARN_LOG << "getClientStubByDeviceId: " << deviceid << " failed";
        return;
    }
    stub_ptr->loginReply(nullptr, request, nullptr, nullptr);
}

DEFINE_SERVICE_RPC(GameClientService, callEntityRpc, proto::RpcMessage)
{
    INFO_LOG << "callEntityRpc method " << request->method() << " " << request->id()
             << " params: " << request->params();
    if (!Global::gate_server)
    {
        WARN_LOG << "gate_server is nullptr";
        return;
    }
    const std::string& deviceid = request->device_info().deviceid();
    auto stub_ptr = Global::gate_server->getClientStubByDeviceId(deviceid);
    if (!stub_ptr)
    {
        WARN_LOG << "getRandomClientStub failed";
        return;
    }
    stub_ptr->callEntityRpc(nullptr, request, nullptr, nullptr);
}

IServicePtr GameClientServiceFactory::createService()
{
    return std::make_shared<GameClientService>();
}

/* vim: set ts=4 sw=4 sts=4 tw=100 */
