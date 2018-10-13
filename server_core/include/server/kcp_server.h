/***************************************************************************
 *
 * Copyright (c) 2018 hpghy, Inc. All Rights Reserved
 * $Id$
 *
 **************************************************************************/

/**
* @file kcp_server.h
* @author hpghy(790042744@qq.com)
* @date 2018/07/29 13:42:56
* @version $Revision$
* @brief
*
**/
#ifndef KCP_SERVER_H
#define KCP_SERVER_H

#include <unordered_map>
#include <memory>

#include <boost/asio.hpp>
#include "server/server.h"
#include "util/util.h"
#include "connection/kcp_connection.h"

class KcpServer: public Server
{
    public:
        explicit KcpServer(boost::asio::io_service&);
        ~KcpServer();

        void bind(const std::string& ip, uint16_t port);
        void listen();
        void start();
        void stop();

        using KcpConnWptr = std::weak_ptr<KcpConnection>;

    private:
        void asyncReceive();
        void handleReceive(const boost::system::error_code&, std::size_t);
        void handleSyncRequest();
        void handleAckRequest();
        void setOption();

    private:
        udp_addr     bind_addr_;
        udp_addr     rmt_addr_;
        udp_sock     socket_;
        std::array<char, 2048> recv_buffer_{{0}};
};
using KcpServerPtr = std::shared_ptr<KcpServer>;


#endif  // KCP_SERVER_H

/* vim: set ts=4 sw=4 sts=4 tw=100 */
