/***************************************************************************
 *
 * Copyright (c) 2018 hpghy, Inc. All Rights Reserved
 * $Id$
 *
 **************************************************************************/

/**
* @file connection.h
* @author hpghy(790042744@qq.com)
* @date 2018/07/29 14:03:48
* @version $Revision$
* @brief
*
**/
#ifndef CONNECTION_H
#define CONNECTION_H

#include <memory>
#include <string>
#include <boost/noncopyable.hpp>
#include <boost/asio.hpp>
#include <google/protobuf/service.h>
#include "../util/util.h"

namespace pb = ::google::protobuf;
class RpcChannel;
class Server;

class Connection: private boost::noncopyable, public std::enable_shared_from_this<Connection>
{
    public:
        using StrandPtr = std::shared_ptr<boost::asio::strand>;

    public:
        Connection(boost::asio::io_service& io_service, std::weak_ptr<Server> server_wptr);
        virtual ~Connection();

        template <typename SocketAddrT>
        void setRemoteAddr(const SocketAddrT& addr)
        {
            rmt_ip_ = addr.address().to_string();
            rmt_port_ = addr.port();
            rmt_addr_ = rmt_ip_ + ":" + std::to_string(rmt_port_);
        }

        // connection创建时在主线程中初始化, TODO...做成init接口更好
        void setChannel(std::shared_ptr<RpcChannel> channel_ptr) { channel_wptr_ = channel_ptr; }
        // 各个线程都可以调用getChannel
        std::weak_ptr<RpcChannel> getChannel() { return channel_wptr_; }

        // 主线程执行
        virtual void startWork();
        // io线程执行
        virtual void doStartWork() = 0;

        // 主线程执行，对外的写接口
        int32_t asyncSend(std::shared_ptr<std::string> data_ptr);

        // 主线程io线程都可以执行
        bool closeSocket();

        // 主线程执行
        void disconnect();
        // io线程执行
        void doDisconnect();

        // 主线程里面执行
        virtual void onConnected();
        virtual void onDisconnected();
        // io线程里面执行
        void notifyDisconnected();

        //client interface
        virtual void asyncConnect() {}

    public:
        const std::string& getRemoteAddr() const { return rmt_addr_; }
        const std::string& getRemoteIp() const { return rmt_ip_; }
        uint16_t getRemotePort() const { return rmt_port_; }
        std::weak_ptr<Server> getServer() { return server_wptr_; }

    protected:
        virtual void setOption() = 0;

        // io线程中执行
        virtual void asyncReceive() = 0;
        virtual void handleReceive(const boost_err&, std::size_t) = 0;

        // io线程中执行
        virtual void doAsyncSendData(std::shared_ptr<std::string>) = 0;
        virtual void handleCloseSocket() = 0;

    protected:
        boost::asio::io_service& getIoService() { return io_service_; }
        bool isClosed() const { return closed_.load(); }
        StrandPtr& getRecvStrand() { return rcv_strand_; }
        StrandPtr& getSendStrand() { return snd_strand_; }

    private:
        boost::asio::io_service&	io_service_;
        uint16_t 				    rmt_port_ = 0;
        std::string 				rmt_ip_;
        std::string 				rmt_addr_;
        std::weak_ptr<Server>       server_wptr_;
        std::weak_ptr<RpcChannel>	channel_wptr_;

        bool 		is_disconnected_ = false;
        std::atomic<bool>		closed_{false};
        StrandPtr 				rcv_strand_;
        StrandPtr 				snd_strand_;
};
using ConnectionPtr = std::shared_ptr<Connection>;

#endif  // CONNECTION_H

/* vim: set ts=4 sw=4 sts=4 tw=100 */
