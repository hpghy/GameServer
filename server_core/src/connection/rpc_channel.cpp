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


RpcChannel::RpcChannel(ConnectionPtr conn_ptr)
    : conn_ptr_(conn_ptr),
      parser_ptr_(std::make_shared<RequestParser>())
{
    INFO_LOG << "Create RpcChannel...";
}

const std::string& RpcChannel::getRemoteAddr() const
{
    if (conn_ptr_)
    {
        return conn_ptr_->getRemoteAddr();
    }
    return "";
}

const std::string& RpcChannel::getRemoteIp() const
{
    if (conn_ptr_)
    {
        return conn_ptr_->getRemoteIp();
    }
    return "";
}

uint16_t RpcChannel::getRemotePort() const
{
    if (conn_ptr_)
    {
        return conn_ptr_->getRemotePort();
    }
    return 0;
}

void RpcChannel::setService(IServicePtr service_ptr)
{
    service_wptr_ = service_ptr;
}
IServiceWptr RpcChannel::getService()
{
    return service_wptr_;
}

void RpcChannel::CallMethod(const pb::MethodDescriptor* method,
                            pb::RpcController*,
                            const pb::Message* request,
                            pb::Message*,
                            pb::Closure*)
{
    bool ret;
    std::shared_ptr<std::string> buf;
    std::tie(ret, buf) = parser_ptr_->unparse(method, request);
    if (!ret)
    {
        return;
    }
    conn_ptr_->asyncSend(buf);
}

bool RpcChannel::handleData(const char* data, std::size_t size)
{
    using state_t = RequestParser::parser_state;

    state_t state;
    std::size_t consume = 0;
    const char* pdata = data;
    std::size_t remain = size;
    while (remain > 0)
    {
        std::tie(state, consume) = parser_ptr_->parse(pdata, remain);
        remain -= consume;
        pdata += consume;
        if (state == state_t::state_request)
        {
            Request& req = parser_ptr_->get_request();
            onRequest(req);
            parser_ptr_->reset_request();
        }
        else if (state == state_t::state_error)
        {
            return false;
        }
    }
    return true;
}

bool RpcChannel::onRequest(Request& request)
{
    auto service_ptr = service_wptr_.lock();
    if (!service_ptr)
    {
        WARN_LOG << "service_ptr_.lock() failed";
        return false;
    }
    const google::protobuf::ServiceDescriptor* descriptor = service_ptr->getDescriptor();
    uint16_t index = -1;
    request.data_istream().read(reinterpret_cast<char*>(&index), sizeof(index));
    if (index >= descriptor->method_count() || index < 0)
    {
        WARN_LOG << "index: " << index << " invalid!";
        return false;
    }

    const pb::MethodDescriptor* method = descriptor->method(index);
    pb::Message* message = service_ptr->getRequestProtoType(method).New();
    message->ParseFromIstream(&request.data_istream());
    onRpcMessage(method, message);

    return true;
}

void RpcChannel::onRpcMessage(const pb::MethodDescriptor* method, pb::Message* message)
{
    QueueRequest* que_request = new QueueServiceRequest(method, message, service_wptr_);
    RequestQueueInst::instance().pushRequest(que_request);
}

/* vim: set ts=4 sw=4 sts=4 tw=100 */
