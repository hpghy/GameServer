/***************************************************************************
 *
 * Copyright (c) 2018 hpghy, Inc. All Rights Reserved
 * $Id$
 *
 **************************************************************************/

/**
* @file rpc_channel.cpp
* @author hpghy(790042744@qq.com)
* @date 2018/07/29 14:19:52
* @version $Revision$
* @brief
*
**/

#ifdef __APPLE__
#define BOOST_LOG_DYN_LINK 1
#endif

#include "connection/rpc_channel.h"

#include "connection/connection.h"
#include "service/rpc_service.h"
#include "service/request_queue.h"
#include "util/util.h"


RpcChannel::RpcChannel(ConnectionPtr conn)
    : conn_(conn),
      parser_(std::make_shared<request_parser>())
{
    INFO_LOG << "Create RpcChannel...";
}

RpcChannel::~RpcChannel()
{
    if (conn_)
    {
        INFO_LOG << "destroy RpcChannel " << conn_->getRemoteAddr();
        conn_.reset();
    }
    service_ptr_.reset();
}

const std::string& RpcChannel::getRemoteAddr() const
{
    if (conn_)
    {
        return conn_->getRemoteAddr();
    }
    return "";
}

const std::string& RpcChannel::getRemoteIp() const
{
    if (conn_)
    {
        return conn_->getRemoteIp();
    }
    return "";
}

uint16_t RpcChannel::getRemotePort() const
{
    if (conn_)
    {
        return conn_->getRemotePort();
    }
    return 0;
}

void RpcChannel::setService(IServicePtr service)
{
    service_ptr_ = service;
}
IServiceWptr RpcChannel::getService()
{
    return service_ptr_;
}

void RpcChannel::CallMethod(const pb::MethodDescriptor* method,
                            pb::RpcController*,
                            const pb::Message* request,
                            pb::Message*,
                            pb::Closure*)
{
    bool ret;
    std::shared_ptr<std::string> buf;
    std::tie(ret, buf) = parser_->unparse(method, request);
    if (!ret)
    {
        return;
    }
    conn_->asyncSend(buf);
}

bool RpcChannel::handleData(const char* data, std::size_t size)
{
    using state_t = request_parser::parser_state;

    state_t state;
    std::size_t consume = 0;
    const char* pdata = data;
    std::size_t remain = size;
    while (remain > 0)
    {
        std::tie(state, consume) = parser_->parse(pdata, remain);
        remain -= consume;
        pdata += consume;
        if (state == state_t::state_request)
        {
            request& req = parser_->get_request();
            onRequest(req);
            parser_->reset_request();
        }
        else if (state == state_t::state_error)
        {
            return false;
        }
    }
    return true;
}

bool RpcChannel::onRequest(request& request)
{
    auto service = service_ptr_.lock();
    if (!service)
    {
        WARN_LOG << "service_ptr_.lock() failed";
        return false;
    }
    const google::protobuf::ServiceDescriptor* descriptor = service->getDescriptor();
    uint16_t index = -1;
    request.data_istream().read(reinterpret_cast<char*>(&index), sizeof(index));
    if (index >= descriptor->method_count() || index < 0)
    {
        WARN_LOG << "index: " << index << " invalid!";
        return false;
    }

    const pb::MethodDescriptor* method = descriptor->method(index);
    pb::Message* message = service->getRequestProtoType(method).New();
    message->ParseFromIstream(&request.data_istream());
    onRpcMessage(method, message);

    return true;
}

void RpcChannel::onRpcMessage(const pb::MethodDescriptor* method, pb::Message* message)
{
    QueueRequest* que_request = new QueueServiceRequest(method, message, service_ptr_);
    RequestQueueInst::instance().pushRequest(que_request);
}

/* vim: set ts=4 sw=4 sts=4 tw=100 */
