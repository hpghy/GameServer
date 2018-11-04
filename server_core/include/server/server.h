/***************************************************************************
 *
 * Copyright (c) 2018 hpghy, Inc. All Rights Reserved
 * $Id$
 *
 **************************************************************************/

/**
* @file server.h
* @author hpghy(790042744@qq.com)
* @date 2018/07/29 13:44:55
* @version $Revision$
* @brief
*
**/
#ifndef SERVER_H
#define SERVER_H

#include <memory>
#include <cstdint>
#include <boost/asio.hpp>

class MobileServer;
class Connection;
using ConnectionPtr = std::shared_ptr<Connection>;

class Server: private boost::noncopyable, public std::enable_shared_from_this<Server>
{
    public:
        using MobileServerWPtr = std::weak_ptr<MobileServer>;

    public:
        Server(boost::asio::io_service&);
        virtual ~Server() = default;

        inline void setMobileServer(std::shared_ptr<MobileServer> mobile_server_ptr)
        {
            mobile_server_wptr_ = mobile_server_ptr;
        }
        inline MobileServerWPtr getMobileServer()
        {
            return mobile_server_wptr_;
        }

        // bind/listen/start 在io线程创建前执行
        virtual void bind(const std::string& ip, uint16_t port);
        virtual void listen() = 0;
        virtual void start();

        // 主线程中执行, io线程结束后调用
        virtual void stop();

        // not thread safe, but not effect.
        bool isStopped() { return stopped_; }

        // 在主线程中执行
        virtual void onConnected(ConnectionPtr conn_ptr);
        virtual void onDisconnected(ConnectionPtr conn_ptr);

    protected:
        boost::asio::io_service& getIoService() { return io_service_; }
        const std::string& getHost() const { return host_; }
        uint16_t getPort() const { return port_; }

    private:
        boost::asio::io_service&        io_service_;
        MobileServerWPtr     mobile_server_wptr_;
        std::string          host_;
        uint16_t             port_;
        volatile bool        stopped_ = true;       // 主线程io线程都会访问
};
using ServerPtr = std::shared_ptr<Server>;
using ServerWptr = std::weak_ptr<Server>;

#endif  // SERVER_H

/* vim: set ts=4 sw=4 sts=4 tw=100 */
