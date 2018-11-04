/***************************************************************************
 *
 * Copyright (c) 2018 hpghy, Inc. All Rights Reserved
 * $Id$
 *
 **************************************************************************/

/**
* @file rpc_service.h
* @author hpghy(790042744@qq.com) * @date 2018/07/29 14:31:19
* @version $Revision$
* @brief
*
**/
#ifndef RPC_SERVICE_H
#define RPC_SERVICE_H

#include <memory>
#include <tuple>
#include <iostream>
#include <boost/lockfree/queue.hpp>
#include <google/protobuf/service.h>
#include "util/util.h"
#include "connection/rpc_channel.h"

#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wdeprecated-declarations"
#include "proto/common.pb.h"
#pragma clang diagnostic pop

namespace pb = ::google::protobuf;

#define DECLARE_SERVICE_RPC(METHOD, REQUEST)    \
    virtual void METHOD(pb::RpcController*,    \
                        const REQUEST* request,    \
                        proto::None*,    \
                        pb::Closure*);

#define DEFINE_SERVICE_RPC(CLASS, METHOD, REQUEST)    \
    void CLASS::METHOD(pb::RpcController*,    \
                        const REQUEST* request,    \
                        proto::None*,    \
                        pb::Closure*)

class IService: public std::enable_shared_from_this<IService>
{
    public:
        IService() {}
        virtual ~IService() = default;

        virtual void setChannel(RpcChannelPtr channel_ptr)
        {
            if (!channel_ptr)
            {
                WARN_LOG << "channel is nullptr";
                return;
            }
            channel_ptr_ = channel_ptr;
        }

        virtual const pb::ServiceDescriptor* getDescriptor() = 0;
        virtual const pb::Message& getRequestProtoType(const pb::MethodDescriptor*) const = 0;

        // 处理来自peer的rpc请求
        virtual void dispatchMethod(const pb::MethodDescriptor* method,
                                    pb::RpcController* controller,
                                    const pb::Message* request,
                                    pb::Message* response,
                                    pb::Closure* done) = 0;

        // 作为客户端使用
        // 连接成功或是失败
        virtual void onConnected(bool) {};
        // 中途断开连接
        virtual void onDisconnected() {};

    protected:
        RpcChannelPtr       channel_ptr_;
};
using IServicePtr = std::shared_ptr<IService>;
using IServiceWptr = std::weak_ptr<IService>;

template<typename ServiceT, typename StubT>
class RpcService: public IService,
    public ServiceT
{
    public:
        using Stub = StubT;
        using StubPtr = std::shared_ptr<StubT>;

    public:
        virtual void setChannel(RpcChannelPtr channel_ptr)
        {
            if (!channel_ptr)
            {
                return;
            }
            IService::setChannel(channel_ptr);
            stub_ptr_ = std::make_shared<StubT>(channel_ptr.get());
        }

        virtual const pb::ServiceDescriptor* getDescriptor()
        {
            return ServiceT::GetDescriptor();
        }

        virtual const pb::Message& getRequestProtoType(const pb::MethodDescriptor* method) const
        {
            return ServiceT::GetRequestPrototype(method);
        }

        // 分发来自client的rpc请求
        virtual void dispatchMethod(const pb::MethodDescriptor* method,
                                    pb::RpcController* controller,
                                    const pb::Message* request,
                                    pb::Message*,
                                    pb::Closure*)
        {
            ServiceT::CallMethod(method, controller, request, nullptr, nullptr);
        }

        // 使用Stub时肯定需要知道Stub的类型，这是业务相关的，不能在Service基类中
        StubPtr getStub()
        {
            return stub_ptr_;
        }

    protected:
        StubPtr   stub_ptr_;
};

// 具体的工厂生成具体的业务service
class ServiceFactory: public boost::noncopyable,
    public std::enable_shared_from_this<ServiceFactory>
{
    public:
        virtual IServicePtr createService() = 0;
};
using ServiceFactoryPtr = std::shared_ptr<ServiceFactory>;

#endif  // RPC_SERVICE_H

/* vim: set ts=4 sw=4 sts=4 tw=100 */
