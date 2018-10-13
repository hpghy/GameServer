/***************************************************************************
 *
 * Copyright (c) 2018 hpghy, Inc. All Rights Reserved
 * $Id$
 *
 **************************************************************************/

/**
* @file rpc_channel.h
* @author hpghy(790042744@qq.com)
* @date 2018/07/29 14:06:34
* @version $Revision$
* @brief
*
**/
#ifndef RPC_CHANNEL_H
#define RPC_CHANNEL_H

#include <memory>
#include <google/protobuf/service.h>
#include "service/request_parser.h"
#include "util/util.h"

namespace pb = ::google::protobuf;

class Connection;
using ConnectionPtr = std::shared_ptr<Connection>;
class IService;
using IServicePtr = std::shared_ptr<IService>;
using IServiceWptr = std::weak_ptr<IService>;

class RpcChannel: public pb::RpcChannel,
    public std::enable_shared_from_this<RpcChannel>
{
    public:
        RpcChannel(ConnectionPtr conn);
        virtual ~RpcChannel();

        const std::string& getRemoteAddr() const;
        const std::string& getRemoteIp() const;
        uint16_t getRemotePort() const;

        // 主线程执行
        void setService(IServicePtr service);
        IServiceWptr getService();
        ConnectionPtr getConnection() { return conn_; }

        virtual void CallMethod(const pb::MethodDescriptor* method,
                                pb::RpcController* controller,
                                const pb::Message* request,
                                pb::Message* response,
                                pb::Closure* done);

        bool handleData(const char*, std::size_t);
        bool onRequest(request&);
        virtual void onRpcMessage(const pb::MethodDescriptor* method, pb::Message* message);

    protected:
        IServiceWptr 	service_ptr_;
        ConnectionPtr 	conn_;
        std::shared_ptr<request_parser>		parser_;
};
using RpcChannelPtr = std::shared_ptr<RpcChannel>;

#endif  // RPC_CHANNEL_H

/* vim: set ts=4 sw=4 sts=4 tw=100 */
