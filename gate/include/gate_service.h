/***************************************************************************
 *
 * Copyright (c) 2018, Inc. All Rights Reserved
 * $Id$
 *
 **************************************************************************/

/**
* @file gate_service.h
* @author hpghy(790042744@qq.com)
* @date 2018/07/01 21:40:42
* @version $Revision$
* @brief
* **/

#ifndef GATE_SERVICE_H
#define GATE_SERVICE_H

#include "service/rpc_service.h"
#include "gate_config.h"

#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wdeprecated-declarations"
#include "proto/client_gate.pb.h"
#pragma clang diagnostic pop

using BaseClass = RpcService<proto::IGateService, proto::IClientService_Stub>;
class GateService : public BaseClass
{
    public:
        GateService();
        ~GateService();

        const std::string& getDeviceId() { return device_client_.deviceid; }

        DECLARE_SERVICE_RPC(seedRequest, proto::None)
        DECLARE_SERVICE_RPC(sessionKeyRequest, proto::EncryptKeyStr)
        DECLARE_SERVICE_RPC(loginRequest, proto::LoginRequest)
        DECLARE_SERVICE_RPC(callEntityRpc, proto::RpcMessage)

    private:
        void setDeviceClient(const proto::DeviceInfo& device_info);

    private:
        DeviceClient device_client_;
};

class GateServiceFactory : public ServiceFactory
{
    public:
        IServicePtr createService() override;
};

#endif  // GATE_SERVICE_H

/* vim: set ts=4 sw=4 sts=4 tw=100 */
