/***************************************************************************
 *
 * Copyright (c) 2018 hpghy, Inc. All Rights Reserved
 * $Id$
 *
 **************************************************************************/

/**
* @file server.cpp
* @author hpghy(790042744@qq.com)
* @date 2018/07/29 14:10:53
* @version $Revision$
* @brief
*
**/

#include "server/server.h"

#include "util/util.h"
#include "connection/connection.h"
#include "server/mobile_server.h"

Server::Server(boost::asio::io_service& io_service)
    : io_service_(io_service)
{
}

void Server::bind(const std::string& ip, uint16_t port)
{
    host_ = ip;
    port_ = port;
}

void Server::start()
{
    stopped_ = false;
}

void Server::stop()
{
    stopped_ = true;
}

void Server::onConnected(ConnectionPtr conn_ptr)
{
    DEBUG_LOG << "Server::onConnected";
    auto mobile_server_ptr = mobile_server_wptr_.lock();
    if (!mobile_server_ptr)
    {
        ERROR_LOG << "mobile_server is null";
        return;
    }
    mobile_server_ptr->onConnected(conn_ptr);
}

void Server::onDisconnected(ConnectionPtr conn_ptr)
{
    DEBUG_LOG << "Server::onDisconnected";
    auto mobile_server_ptr = mobile_server_wptr_.lock();
    if (!mobile_server_ptr)
    {
        ERROR_LOG << "mobile_server is null";
        return;
    }
    mobile_server_ptr->onDisconnected(conn_ptr);
}

/* vim: set ts=4 sw=4 sts=4 tw=100 */
