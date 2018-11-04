/***************************************************************************
 *
 * Copyright (c) 2018 hpghy, Inc. All Rights Reserved
 * $Id$
 *
 **************************************************************************/

/**
* @file service_manager.cpp
* @author hpghy(790042744@qq.com)
* @date 2018/09/08 18:53:32
* @version $Revision$
* @brief
*
**/

#include "service/service_manager.h"

#include <random>
#include "connection/rpc_channel.h"

void ServiceManager::onConnected(ConnectionPtr conn_ptr)
{
    if (!conn_ptr)
    {
        return;
    }
    if (!factory_ptr_)
    {
        ERROR_LOG << "factory_ is nullptr";
        return;
    }
    const std::string& rmt_addr = conn_ptr->getRemoteAddr();
    auto iter = service_map_.find(rmt_addr);
    if (iter != service_map_.end())
    {
        ERROR_LOG << "rmt_addr " << rmt_addr << " is still exist!!!";
        return;
    }
    auto channel_ptr = std::make_shared<RpcChannel>(conn_ptr);
    auto service_ptr = factory_ptr_->createService();
    service_ptr->setChannel(channel_ptr);
    service_map_.emplace(rmt_addr, service_ptr);
    channel_ptr->setService(service_ptr);
    conn_ptr->setChannel(channel_ptr);

    INFO_LOG << "ServiceManager add " << rmt_addr;
}

void ServiceManager::onDisconnected(ConnectionPtr conn_ptr)
{
    if (!conn_ptr)
    {
        DEBUG_LOG << "onDisconnected conn is nullptr";
        return;
    }
    const std::string& rmt_addr = conn_ptr->getRemoteAddr();
    service_map_.erase(rmt_addr);

    INFO_LOG << "ServiceManager erase " << rmt_addr;
}

IServicePtr ServiceManager::getService(ConnectionPtr conn_ptr)
{
    if (!conn_ptr)
    {
        return IServicePtr();
    }
    return getService(conn_ptr->getRemoteAddr());
}

IServicePtr ServiceManager::getService(const std::string& rmt_addr)
{
    auto iter = service_map_.find(rmt_addr);
    if (iter != service_map_.end())
    {
        return iter->second;
    }
    return IServicePtr();
}

IServicePtr ServiceManager::getRandomService()
{
    std::size_t size = service_map_.size();
    if (0 == size)
    {
        return IServicePtr();
    }
    std::default_random_engine e;
    std::uniform_int_distribution<unsigned> u(0, size);
    auto iter = service_map_.begin();
    std::advance(iter, u(e));
    return iter->second;
}

/* vim: set ts=4 sw=4 sts=4 tw=100 */
