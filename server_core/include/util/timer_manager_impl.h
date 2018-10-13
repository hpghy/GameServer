/***************************************************************************
 * 
 * Copyright (c) 2018 hpghy, Inc. All Rights Reserved
 * $Id$ 
 * 
 **************************************************************************/
 
 /**
 * @file timer_manager_impl.h
 * @author hpghy(790042744@qq.com)
 * @date 2018/07/29 21:19:44
 * @version $Revision$ 
 * @brief 
 *  
 **/
#ifndef TIMER_MANAGER_IMPL_H
#define TIMER_MANAGER_IMPL_H

#include <memory>
#include <ctime>
#include <boost/multi_index_container.hpp>
#include <boost/multi_index/member.hpp>
#include <boost/multi_index/ordered_index.hpp>
#include <boost/multi_index/hashed_index.hpp>
#include "util.h"

typedef void (*CallbackFuncPtr)();

class TimerCallback
{
public:
    virtual ~TimerCallback() {}
    virtual void call() const = 0;
};
using TimerCallbackUPtr = std::unique_ptr<TimerCallback>; 
using TimerCallbackPtr = std::shared_ptr<TimerCallback>; 

class TimerFuncPtrCallback: public TimerCallback
{
public:
    TimerFuncPtrCallback(CallbackFuncPtr f): func_ptr_(f)
    {}

    ~TimerFuncPtrCallback()
    {
        func_ptr_ = nullptr;
    }

    void call() const
    {
        if (func_ptr_)
        {
            (*func_ptr_)();
        }
    }

private:
    CallbackFuncPtr func_ptr_;
};

template <typename Functor>
class TimerFunctorCallback: public TimerCallback
{
public:
    TimerFunctorCallback(Functor f): f_(f)
    {}
    
    void call() const
    {
        f_();
    }

private:
    Functor f_;
};

template <typename T>
class TimerMemFuncPtrCallback: public TimerCallback
{
public:
    typedef void(T::*F)();

    TimerMemFuncPtrCallback(T* self, F f): self_(self), func_ptr_(f)
    {}

    ~TimerMemFuncPtrCallback()
    {
        self_ = nullptr;
        func_ptr_ = nullptr;
    }

    void call() const
    {
        if (self_ && func_ptr_)
        {
            (self_->*func_ptr_)();
        }
    }
  
private:
    T* self_;
    F func_ptr_;
};

template <typename T>
class TimerWeakMemFuncPtrCallback: public TimerCallback
{
public:
    typedef void(T::*F)();

    TimerWeakMemFuncPtrCallback(std::weak_ptr<T> self, F f): self_(self), func_ptr_(f)
    {}

    ~TimerWeakMemFuncPtrCallback()
    {
        self_.reset();
        func_ptr_ = nullptr;
    }

    void call() const
    {
        std::shared_ptr<T> this_ = self_.lock();
        if (this_ && func_ptr_)
        {
            ((this_.get())->*func_ptr_)();
        }
    }

private:
    std::weak_ptr<T> self_;
    F func_ptr_;
}; 

struct TimerHandle
{
    TimerHandle(uint64_t timer_id, uint32_t expire, TimerCallback* cb_ptr)
        : id(timer_id), expire_time(expire), callback_ptr(cb_ptr)
    {}

    ~TimerHandle()
    {
        callback_ptr.reset();
    }

    void call() const
    {
        if (callback_ptr)
        {
            callback_ptr->call();
        }
    }

    uint64_t id;
    uint32_t expire_time;
    // 不能使用std::unique_ptr, multi_index_container内部有拷贝操作
    TimerCallbackPtr callback_ptr;
};

template <typename TimeStampT>
class TimerManagerImpl
{
public:
    struct time_idx{};
    struct id_idx{};
    using time_tag = boost::multi_index::tag<time_idx>;
    using id_tag = boost::multi_index::tag<id_idx>;
    using TimerContainer = boost::multi_index_container<
        TimerHandle,
        boost::multi_index::indexed_by<
            boost::multi_index::ordered_non_unique<time_tag, boost::multi_index::member<TimerHandle, uint32_t, &TimerHandle::expire_time> >,
            boost::multi_index::hashed_non_unique<id_tag, boost::multi_index::member<TimerHandle, uint64_t, &TimerHandle::id> > 
        > >;

    // 居然要加一个template!!!
    using TimeStampIndex = typename TimerContainer::template nth_index<0>::type;
    using IdIndex = typename TimerContainer::template nth_index<1>::type;

    using TimeStampFunction = std::function<TimeStampT()>;


public:
    TimerManagerImpl(TimeStampFunction f);

    uint64_t addTimer(uint32_t delay, CallbackFuncPtr f);

    template <typename F>
    uint64_t addTimer(uint32_t delay, F f);

    template <typename ObjT>
    uint64_t addTimer(uint32_t, ObjT* self, void(ObjT::*f)());

    template <typename ObjT>
    uint64_t addTimer(uint32_t, std::weak_ptr<ObjT> self, void(ObjT::*f)());

    uint64_t addTimer(uint32_t delay, TimerCallback* cb_ptr);
    
    void delTimer(uint64_t id);

    void tick(uint32_t max_timer);

private:
    inline uint64_t genId() { return gen_id_++; }

    // 居然要加一个template!!!
    // multi_index_container的索引都是不能直接修改元素的
    inline TimeStampIndex& getTimeIndex() { return timer_container_. template get<0>(); }
    inline IdIndex& getIdIndex() { return timer_container_. template get<1>(); }

private:
    uint64_t gen_id_;
    TimeStampFunction timestamp_func_;
    TimerContainer timer_container_;
};

template <typename TimeStampT>
TimerManagerImpl<TimeStampT>::TimerManagerImpl(TimeStampFunction f): gen_id_(1), timestamp_func_(f)
{
    if (!timestamp_func_)
    {
        timestamp_func_ = std::bind(&std::time, nullptr);
    }
}

template <typename TimeStampT>
uint64_t TimerManagerImpl<TimeStampT>::addTimer(uint32_t delay, CallbackFuncPtr f)
{
    TimerCallback* cb_ptr = new TimerFuncPtrCallback(f);
    return addTimer(delay, cb_ptr);
}

template <typename TimeStampT>
template <typename F>
uint64_t TimerManagerImpl<TimeStampT>::addTimer(uint32_t delay, F f)
{
    TimerCallback* cb_ptr = new TimerFunctorCallback<F>(f);
    return addTimer(delay, cb_ptr);
    return 0;
}

template <typename TimeStampT>
template <typename ObjT>
uint64_t TimerManagerImpl<TimeStampT>::addTimer(uint32_t delay, ObjT* self, void(ObjT::*f)())
{
    TimerCallback* cb_ptr = new TimerMemFuncPtrCallback<ObjT>(self, f);
    return addTimer(delay, cb_ptr);
}

template <typename TimeStampT>
template <typename ObjT>
uint64_t TimerManagerImpl<TimeStampT>::addTimer(uint32_t delay, std::weak_ptr<ObjT> self, void(ObjT::*f)())
{
    TimerCallback* cb_ptr = new TimerWeakMemFuncPtrCallback<ObjT>(self, f);
    return addTimer(delay, cb_ptr);
}

template <typename TimeStampT>
uint64_t TimerManagerImpl<TimeStampT>::addTimer(uint32_t delay, TimerCallback* cb_ptr)
{
    auto id = genId();
    auto now = timestamp_func_();
    TimerHandle handle(id, now + delay, cb_ptr);
    auto ret = getTimeIndex().insert(handle);
    if (!ret.second)
    {
        ERROR_LOG << "addTimer id: " << id << " expire " << now + delay << " failed";
    }
    return id;
}

template <typename TimeStampT>
void TimerManagerImpl<TimeStampT>::delTimer(uint64_t id)
{
    auto& id_index = getIdIndex();
    id_index.erase(id);
}

template <typename TimeStampT>
void TimerManagerImpl<TimeStampT>::tick(uint32_t max_timer)
{
    std::vector<uint64_t> del_ids_vec;
    auto now = timestamp_func_();
    uint32_t expire_cnt = 0;
    auto& index = getTimeIndex();
    for (auto& handle: index)
    {
        if (handle.expire_time > now || (max_timer > 0 && expire_cnt >= max_timer))
        {
            break;
        }
        
        ++ expire_cnt;
        del_ids_vec.push_back(handle.id);
        handle.call();
    }

    auto& id_index = getIdIndex();
    for (size_t idx = 0; idx < del_ids_vec.size(); ++idx)
    {
        id_index.erase(del_ids_vec[idx]);
    }
}

#endif  // TIMER_MANAGER_IMPL_H

/* vim: set ts=4 sw=4 sts=4 tw=100 */
