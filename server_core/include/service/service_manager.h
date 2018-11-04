/***************************************************************************
 *
 * Copyright (c) 2018 hpghy, Inc. All Rights Reserved
 * $Id$
 *
 **************************************************************************/

/**
* @file service_manager.h
* @author hpghy(790042744@qq.com)
* @date 2018/09/08 18:46:08
* @version $Revision$
* @brief
*
**/
#ifndef SERVICE_MANAGER_H
#define SERVICE_MANAGER_H

#include <unordered_map>
#include "service/rpc_service.h"
#include "connection/connection.h"

class ServiceManager
{
    public:
        ServiceManager(ServiceFactoryPtr factory_ptr) : factory_ptr_(factory_ptr)
        {}

        void onConnected(ConnectionPtr conn_ptr);
        void onDisconnected(ConnectionPtr conn_ptr);
        IServicePtr getService(ConnectionPtr conn_ptr);
        IServicePtr getService(const std::string& rmt_addr);
        IServicePtr getRandomService();

    private:
        ServiceFactoryPtr factory_ptr_;
        std::unordered_map<std::string, IServicePtr> service_map_;
};

#endif  // SERVICE_MANAGER_H

/* vim: set ts=4 sw=4 sts=4 tw=100 */
