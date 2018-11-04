/***************************************************************************
 *
 * Copyright (c) 2018, Inc. All Rights Reserved
 * $Id$
 *
 **************************************************************************/

/**
* @file game_server.h
* @author hpghy(790042744@qq.com)
* @date 2018/07/01 21:18:39
* @version $Revision$
* @brief
*
**/
#ifndef GAME_SERVER_H
#define GAME_SERVER_H

#include "server/mobile_server.h"
#include "game_config.h"
using namespace std;

class TcpServer;
using TcpServerPtr = shared_ptr<TcpServer>;
class HttpServer;
using HttpServerPtr = shared_ptr<HttpServer>;

class GameServer: public MobileServer
{
    public:
        GameServer(const GameConfig& game_config, const ServerConfig& server_config);
        virtual ~GameServer();

    protected:
        virtual void startServer();
        virtual void stopServer();
        virtual void onReloadConfig();

    private:
        GameConfig config_;
        ServerConfig self_config_;
        TcpServerPtr tcp_server_ptr_;
        HttpServerPtr http_server_ptr_;
};

#endif  // GAME_SERVER_H

/* vim: set ts=4 sw=4 sts=4 tw=100 */
