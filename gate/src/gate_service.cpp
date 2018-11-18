/***************************************************************************
 *
 * Copyright (c) 2018 hpghy, Inc. All Rights Reserved
 * $Id$
 *
 **************************************************************************/
/**
* @file gate_service.cpp
* @author hpghy(790042744@qq.com)
* @date 2018/07/01 21:44:50
* @version $Revision$
* @brief
*
**/

#include "gate_service.h"

#include<boost/core/ignore_unused.hpp>
#include "gate_server.h"
#include "global.h"

GateService::GateService()
{
    INFO_LOG << "Create GateService....";
}

GateService::~GateService()
{
    INFO_LOG << "Destroy GateService...";
}

DEFINE_SERVICE_RPC(GateService, seedRequest, proto::None)
{
    boost::ignore_unused(request);
    INFO_LOG << "in seed_request";
}

DEFINE_SERVICE_RPC(GateService, sessionKeyRequest, proto::EncryptKeyStr)
{
    boost::ignore_unused(request);
    INFO_LOG << "in session_key";
}

DEFINE_SERVICE_RPC(GateService, loginRequest, proto::LoginRequest)
{
    const std::string& deviceid = request->device_info().deviceid();
    INFO_LOG << "in loginRequest deviceid: " << deviceid;
    setDeviceClient(request->device_info());
    auto game_stub_ptr = Global::gate_server->getGameStubByDeviceId(deviceid);
    if (!game_stub_ptr)
    {
        WARN_LOG << "getGameStub for deviceid: " << deviceid << " return nullptr";
        return;
    }
    game_stub_ptr->loginRequest(nullptr, request, nullptr, nullptr);
}

void GateService::setDeviceClient(const proto::DeviceInfo& device_info)
{
    device_client_.addr = channel_ptr_->getRemoteAddr();
    device_client_.deviceid = device_info.deviceid();
    device_client_.session_id = 0;
    device_client_.game_name = Global::gate_server->getRandomGameName();

    INFO_LOG << "setDeviceClient addr: " << device_client_.addr
             << " deviceid: " << device_client_.deviceid
             << " gamename: " << device_client_.game_name;

    Global::gate_server->addDeviceClient(device_client_);
}

DEFINE_SERVICE_RPC(GateService, callEntityRpc, proto::RpcMessage)
{
    INFO_LOG << channel_ptr_->getRemoteAddr() << " in call_entity_method " << request->id()
             << " " << request->method() << " " << request->params();
    if (!Global::gate_server)
    {
        WARN_LOG << "gate_server is nullptr";
        return;
    }
    auto game_stub_ptr = Global::gate_server->getGameStubByDeviceId(device_client_.deviceid);
    if (!game_stub_ptr)
    {
        WARN_LOG << "getRandomGameStub return null";
    }
    proto::RpcMessage msg;
    msg.set_method("rpc_s_hello_from_gate");
    msg.set_id(request->id());
    msg.set_params(request->params());
    auto* device_info_ptr = msg.mutable_device_info();
    if (!device_info_ptr)
    {
        WARN_LOG << "mutable_device_info failed";
        return;
    }
    device_info_ptr->set_addr(device_client_.addr);
    device_info_ptr->set_deviceid(device_client_.deviceid);
    device_info_ptr->set_sessionid(device_client_.session_id);
    game_stub_ptr->callEntityRpc(nullptr, &msg, nullptr, nullptr);
}

/* vim: set ts=4 sw=4 sts=4 tw=100 */
