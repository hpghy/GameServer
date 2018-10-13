/***************************************************************************
 *
 * Copyright (c) 2018 hpghy, Inc. All Rights Reserved
 * $Id$
 *
 **************************************************************************/

/**
* @file client_service.h
* @author hpghy(790042744@qq.com)
* @date 2018/07/15 17:48:42
* @version $Revision$
* @brief
*
**/
#ifndef CLIENT_SERVICE_H
#define CLIENT_SERVICE_H

#include <memory>
#include <boost/asio.hpp>

#include "service/rpc_service.h"
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wdeprecated-declarations"
#include "proto/client_gate.pb.h"
#pragma clang diagnostic pop

#include "connection/tcp_client.h"

using BaseClass = RpcService<proto::IClientService, proto::IGateService_Stub>;
class ClientService: public BaseClass
{
    public:
        ClientService();
        ~ClientService();

        DECLARE_SERVICE_RPC(seedReply, proto::None)
        DECLARE_SERVICE_RPC(sessionKeyReply, proto::None)
        DECLARE_SERVICE_RPC(loginReply, proto::LoginReply)
        DECLARE_SERVICE_RPC(callEntityRpc, proto::RpcMessage)

        void setParam(const std::string& param) { this->param = param; }

    private:
        void doTest();

    private:
        uint32_t test_num_ = 0;
        std::string param;
};
using ClientServicePtr = std::shared_ptr<ClientService>;

class ClientServiceFactory: public ServiceFactory
{
    public:
        virtual IServicePtr createService()
        {
            return std::static_pointer_cast<IService>(std::make_shared<ClientService>());
        }
};

#endif  // CLIENT_SERVICE_H

/* vim: set ts=4 sw=4 sts=4 tw=100 */
