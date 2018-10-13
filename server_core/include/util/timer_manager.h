/***************************************************************************
 * 
 * Copyright (c) 2018 hpghy, Inc. All Rights Reserved
 * $Id$ 
 * 
 **************************************************************************/
 
 /**
 * @file timer_manager.h
 * @author hpghy(790042744@qq.com)
 * @date 2018/07/29 21:18:59
 * @version $Revision$ 
 * @brief 
 *  
 **/
#ifndef TIMER_MANAGER_H
#define TIMER_MANAGER_H

#include <memory>
#include <ctime>
#include <boost/container/detail/singleton.hpp>
#include "timer_manager_impl.h"

class TimerManager;
class ProxyHandle: public std::enable_shared_from_this<ProxyHandle>
{
public:
    ProxyHandle(uint64_t id, uint32_t delay, TimerManager* timer_mgr)
        : delay_(delay), remain_cnt_(1), id_(id), timer_mgr_ptr_(timer_mgr)
    {} 
    ~ProxyHandle();

    void cancel() const;

    uint64_t getId() const { return id_; }

private:
    uint32_t delay_;
    uint32_t remain_cnt_;
    uint64_t id_;
    TimerManager* timer_mgr_ptr_;
};
using ProxyHandlePtr = std::shared_ptr<ProxyHandle>;

template <typename TimeStampT>
class TimerManagerImpl;

class TimerManager
{
    friend boost::container::container_detail::singleton_default<TimerManager>;

private:
    TimerManager();
    ~TimerManager();

public:
    // 一次tick最多执行多少个timer,避免服务器卡顿
    void tick(uint32_t max_timer);

    // 函数指针版本
    typedef void (*FunctionPtr)();
    ProxyHandlePtr addTimer(uint32_t delay, FunctionPtr f);

    // 仿函数对象版本
    template <typename F>
    ProxyHandlePtr addTimer(uint32_t delay, F f)
    {
        uint64_t id = impl_ptr_->addTimer(delay, f);
        return std::make_shared<ProxyHandle>(id, delay, this);
    }

    // 未绑定的函数对象指针 + this
    template <typename T>
    ProxyHandlePtr addTimer(uint32_t delay, T* obj_ptr, void(T::*f)())
    {
        uint64_t id = impl_ptr_->addTimer(delay, obj_ptr, f);
        return std::make_shared<ProxyHandle>(id, delay, this);
    }

    // 未绑定的函数对象指针 + week this
    template <typename T>
    ProxyHandlePtr addTimer(uint32_t delay, std::weak_ptr<T> obj_wptr, void(T::*f)())
    {
        uint64_t id = impl_ptr_->addTimer(delay, obj_wptr, f);
        return std::make_shared<ProxyHandle>(id, delay, this);
    }
    template <typename T>
    ProxyHandlePtr addTimer(uint32_t delay, std::shared_ptr<T> obj_ptr, void(T::*f)())
    {
        auto obj_wptr = std::weak_ptr<T>(obj_ptr);
        uint64_t id = impl_ptr_->addTimer(delay, obj_wptr, f);
        return std::make_shared<ProxyHandle>(id, delay, this);
    }

    void delTimer(const ProxyHandle& handle);
    void delTimer(uint64_t id);

private:
    // 没有办法使用pimpl手法，因为TimerManager必须支持template
    // 必须在template function声明中引用Impl的方法
    std::unique_ptr<TimerManagerImpl<time_t> > impl_ptr_;
};
using TimerManagerInst = boost::container::container_detail::singleton_default<TimerManager>;

#endif  // TIMER_MANAGER_H

/* vim: set ts=4 sw=4 sts=4 tw=100 */
