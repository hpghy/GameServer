/***************************************************************************
 *
 * Copyright (c) 2018 hpghy, Inc. All Rights Reserved
 * $Id$
 *
 **************************************************************************/

/**
* @file mobile_server.h
* @author hpghy(790042744@qq.com)
* @date 2018/07/29 13:41:03
* @version $Revision$
* @brief
*
**/
#ifndef MOBILE_SERVER_H
#define MOBILE_SERVER_H

#include <memory>
#include <string>
#include <thread>
#include <boost/noncopyable.hpp>
#include <boost/asio.hpp>

#include "connection/connection.h"
#include "service/rpc_service.h"
#include "service/service_manager.h"

/**
 * TODO...考虑做成单实例，但是io线程中不能访问
 **/
class MobileServer: public std::enable_shared_from_this<MobileServer>,
    private boost::noncopyable
{
    public:
        const static uint32_t io_thread_num = 3;

    public:
        MobileServer(ServiceFactoryPtr factory);
        virtual ~MobileServer();

        virtual bool start();
        virtual void stop();
        virtual void run();
        virtual void poll();

        // 在主线程执行
        virtual void onConnected(ConnectionPtr conn);
        virtual void onDisconnected(ConnectionPtr conn);

        void onRecvShutdownSignal() { is_shutdown_ = true; }
        void onRecvReloadSignal() { is_reload_config_ = true;  }

        inline boost::asio::io_service& getIoService() { return io_service_; }
        IServicePtr getService(const std::string& rmt_addr) { return service_mgr_.getService(rmt_addr); }

    protected:
        virtual void initBeforeStartServer();
        virtual void startServer();
        virtual void initBeforeStartThread();
        virtual void onReloadConfig();
        virtual void stopServer();

    private:
        boost::asio::io_service io_service_;
        bool is_running_ = false;
        bool is_shutdown_ = false;
        bool is_reload_config_ = false;
        std::shared_ptr<boost::asio::io_service::work> work_;
        std::vector<std::thread> io_threads_;
        ServiceManager service_mgr_;
};

#endif  // MOBILE_SERVER_H

/* vim: set ts=4 sw=4 sts=4 tw=100 */
