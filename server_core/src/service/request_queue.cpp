/***************************************************************************
 *
 * Copyright (c) 2018 hpghy, Inc. All Rights Reserved
 * $Id$
 *
 **************************************************************************/

/**
* @file request_queue.cpp
* @author hpghy(790042744@qq.com)
* @date 2018/09/08 18:44:42
* @version $Revision$
* @brief
*
**/

#include "service/request_queue.h"

#include "service/rpc_service.h"
#include "connection/connection.h"

QueueServiceRequest::QueueServiceRequest(const pb::MethodDescriptor* method, pb::Message* request, IServicePtr service)
    : method_ptr_(method), request_ptr_(request), service_wptr_(service)
{}

QueueServiceRequest::QueueServiceRequest(const pb::MethodDescriptor* method, pb::Message* request, IServiceWptr service)
    : method_ptr_(method), request_ptr_(request), service_wptr_(service)
{}

QueueServiceRequest::~QueueServiceRequest()
{
    method_ptr_ = nullptr;
    if (nullptr != request_ptr_)
    {
        delete request_ptr_;
        request_ptr_ = nullptr;
    }
    service_wptr_.reset();
}

void QueueServiceRequest::call() const
{
    IServicePtr this_ptr = service_wptr_.lock();
    if (this_ptr && method_ptr_ && request_ptr_)
    {
        this_ptr->dispatchMethod(method_ptr_, nullptr, request_ptr_, nullptr, nullptr);
    }
}

RequestQueue::RequestQueue()
    : requests_(5000)
{
    INFO_LOG << "create RequestQueue!!";
}

RequestQueue::~RequestQueue()
{
    INFO_LOG << "Destroy RequestQueue";
}

void RequestQueue::pushRequest(QueueRequest* request)
{
    if (nullptr == request)
    {
        return;
    }
    requests_.push(request);
}

uint32_t RequestQueue::processRequest(uint32_t max_num)
{
    QueueRequest* request = nullptr;
    uint32_t dealed_cnt = 0;
    while (requests_.pop(request) && nullptr != request)
    {
        request->call();
        delete request;
        request = nullptr;

        ++dealed_cnt;
        if (dealed_cnt >= max_num)
        {
            break;
        }
    }
    return dealed_cnt;
}

/* vim: set ts=4 sw=4 sts=4 tw=100 */
