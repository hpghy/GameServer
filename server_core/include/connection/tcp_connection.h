/***************************************************************************
 *
 * Copyright (c) 2018 hpghy, Inc. All Rights Reserved
 * $Id$
 *
 **************************************************************************/

/**
* @file tcp_connection.h
* @author hpghy(790042744@qq.com)
* @date 2018/07/29 14:05:44
* @version $Revision$
* @brief
*
**/
#ifndef TCP_CONNECTION_H
#define TCP_CONNECTION_H

#include <memory>
#include <vector>
#include "connection.h"
#include "../util/util.h"

class Server;

class TcpConnection: public Connection
{
    public:
        using SendingBufferSequence = std::vector<std::shared_ptr<std::string>>;

    public:
        TcpConnection(boost::asio::io_service& io_service, std::weak_ptr<Server> server);
        ~TcpConnection();
        tcp_space::socket& socket() { return socket_; }

        // io线程执行
        void doStartWork() override;
        // io线程执行
        void handleReceive(const boost_err&, std::size_t) override;

    protected:
        void setOption() override;
        void asyncReceive() override;
        void doAsyncSendData(std::shared_ptr<std::string>) override;
        void startAsyncSend();
        void handleAsyncWrite(const boost_err&, std::size_t);
        void handleCloseSocket() override;

    protected:
        tcp_space::socket& getSocket() { return socket_; }
        std::vector<char>& getRecvBuffer() { return recv_buffer_; }

    private:
        tcp_space::socket 		socket_;
        std::vector<char>		recv_buffer_;
        bool 		sending_ = false;
        SendingBufferSequence   data_queue_;			// 缓存待发送的数据
        SendingBufferSequence   sending_queue_;			// 缓存正在异步写的数据, 需要保证异步写时数据还存在
        std::size_t             sending_size_ = 0;      // 异步发送数据大小
};
using TcpConnectionPtr = std::shared_ptr<TcpConnection>;

#endif  // TCP_CONNECTION_H

/* vim: set ts=4 sw=4 sts=4 tw=100 */
