#include "util/util.h"
#include <ctime>
#include "util/timer_manager.h"

ProxyHandle::~ProxyHandle()
{
    remain_cnt_ = 0;
    id_ = 0;
    timer_mgr_ptr_ = nullptr;
}

void ProxyHandle::cancel() const
{
    if (timer_mgr_ptr_ && id_ > 0)
    {
        timer_mgr_ptr_->delTimer(id_);
    }
}

TimerManager::TimerManager()
// 注意C++11并没有定义std::make_unique
    : impl_ptr_(new TimerManagerImpl<time_t>(std::bind(&std::time, nullptr)))
{
    INFO_LOG << "create TimerManager";
}

TimerManager::~TimerManager()
{
    INFO_LOG << "Destroy TimerManager";
}

ProxyHandlePtr TimerManager::addTimer(uint32_t delay, FunctionPtr f)
{
    uint64_t id = impl_ptr_->addTimer(delay, f);
    return std::make_shared<ProxyHandle>(id, delay, this);
}

void TimerManager::delTimer(const ProxyHandle& handle)
{
    delTimer(handle.getId());
}

void TimerManager::delTimer(uint64_t id)
{
    impl_ptr_->delTimer(id);
}

void TimerManager::tick(uint32_t max_timer)
{
    impl_ptr_->tick(max_timer);
}


