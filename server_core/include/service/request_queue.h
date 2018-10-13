/***************************************************************************
 *
 * Copyright (c) 2018 hpghy, Inc. All Rights Reserved
 * $Id$
 *
 **************************************************************************/

/**
* @file request_queue.h
* @author hpghy(790042744@qq.com)
* @date 2018/09/08 18:43:06
* @version $Revision$
* @brief
*
**/
#ifndef REQUEST_QUEUE_H
#define REQUEST_QUEUE_H

#include <unordered_map>
#include <boost/noncopyable.hpp>
#include <boost/lockfree/queue.hpp>
#include <google/protobuf/service.h>
#include <boost/container/detail/singleton.hpp>

#include "util/util.h"

namespace pb = ::google::protobuf;

class IService;
using IServicePtr = std::shared_ptr<IService>;
using IServiceWptr = std::weak_ptr<IService>;

class Connection;
using ConnectionPtr = std::shared_ptr<Connection>;

class ServiceFactory;
using ServiceFactoryPtr = std::shared_ptr<ServiceFactory>;

class QueueRequest
{
    public:
        virtual ~QueueRequest() {}
        virtual void call() const = 0;
};

class QueueServiceRequest: public QueueRequest
{
    public:
        QueueServiceRequest(const pb::MethodDescriptor* method, pb::Message* request, IServicePtr serivce);
        QueueServiceRequest(const pb::MethodDescriptor* method, pb::Message* request, IServiceWptr serivce);
        virtual ~QueueServiceRequest();
        virtual void call() const;

    private:
        const pb::MethodDescriptor* method_;
        pb::Message* request_;
        IServiceWptr service_;
};

class QueueCallbackRequest: public QueueRequest
{
    public:
        using CallbackType = std::function<void()>;

    public:
        QueueCallbackRequest(CallbackType f): f_(f)
        {}

        virtual void call() const
        {
            if (f_)
            {
                f_();
            }
            else
            {
                WARN_LOG << "f_ is NULL";
            }
        }

    private:
        std::function<void()> f_;
};

template <typename T>
class QueueSocketRequest: public QueueRequest
{
    public:
        typedef void(T::*F)();

    public:
        QueueSocketRequest(std::weak_ptr<T> self, F f)
            : self_(self), f_(f)
        {}

        virtual ~QueueSocketRequest()
        {
            self_.reset();
            f_ = nullptr;
        }
        virtual void call() const
        {
            std::shared_ptr<T> this_ptr = self_.lock();
            if (nullptr != this_ptr && nullptr != f_)
            {
                ((this_ptr.get())->*f_)();
            }
        }

    private:
        std::weak_ptr<T> self_;
        F f_;
};

// 主要为业务层使用，工作在主线程上
class RequestQueue: public boost::noncopyable
{
        // singleton_default里面需要调用ServiceManager构造函数
        // 这样子ServiceManagerInst才能无缝地替换ServiceManager, 且防止外界创建新的实例
        friend boost::container::container_detail::singleton_default<RequestQueue>;

    private:
        RequestQueue();
        ~RequestQueue();

    public:
        void pushRequest(QueueRequest* request);
        uint32_t processRequest(uint32_t max_num);

    private:
        boost::lockfree::queue<QueueRequest*> requests_;
};
using RequestQueueInst = boost::container::container_detail::singleton_default<RequestQueue>;

#endif  // REQUEST_QUEUE_H

/* vim: set ts=4 sw=4 sts=4 tw=100 */
