/***************************************************************************
 *
 * Copyright (c) 2018 hpghy, Inc. All Rights Reserved
 * $Id$
 *
 **************************************************************************/

/**
* @file client_service.cpp
* @author hpghy(790042744@qq.com)
* @date 2018/07/15 17:44:41
* @version $Revision$
* @brief
*
**/

#include "client_service.h"

#include "util/util.h"
#include "util/timer_manager.h"

ClientService::ClientService()
{
    INFO_LOG << "Create ClientService";
}

ClientService::~ClientService()
{
    INFO_LOG << "Destroy ClientService";
}

DEFINE_SERVICE_RPC(ClientService, seedReply, proto::None)
{
    INFO_LOG << "in seedReply";
}

DEFINE_SERVICE_RPC(ClientService, sessionKeyReply, proto::None)
{
    INFO_LOG << "in sessionKeyReply";
}

DEFINE_SERVICE_RPC(ClientService, loginReply, proto::LoginReply)
{
    INFO_LOG << "in loginReply, retcode: " << request->retcode();

    std::weak_ptr<ClientService> wptr = std::dynamic_pointer_cast<ClientService>(shared_from_this());
    TimerManagerInst::instance().addTimer(1, wptr, &ClientService::doTest);
}

DEFINE_SERVICE_RPC(ClientService, callEntityRpc, proto::RpcMessage)
{
    INFO_LOG << "in callEntityRpc " << request->id() << " " << request->params() << " " << request->method();
}

void ClientService::doTest()
{
    for (int i = 0; i < 1000; ++i)
    {
        proto::RpcMessage msg;
        msg.set_method(this->param);
        msg.set_id(10001);
        std::string params = this->param + "__" + std::to_string(test_num_);
        msg.set_params(params);
        getStub()->callEntityRpc(nullptr, &msg, nullptr, nullptr);
        DEBUG_LOG << "test num: " << test_num_;
        ++test_num_;
    }

    ++timer_num;
    if (timer_num > 28)
    {
        return;
    }
    std::weak_ptr<ClientService> wptr = std::dynamic_pointer_cast<ClientService>(shared_from_this());
    TimerManagerInst::instance().addTimer(1, wptr, &ClientService::doTest);
}

/* vim: set ts=4 sw=4 sts=4 tw=100 */
