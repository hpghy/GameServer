/***************************************************************************
 *
 * Copyright (c) 2018 hpghy, Inc. All Rights Reserved
 * $Id$
 *
 **************************************************************************/

/**
* @file main.cpp
* @author hpghy(790042744@qq.com)
* @date 2018/07/02 22:07:54
* @version $Revision$
* @brief
*
**/

#include <iostream>
#include <memory>
#include "util/util.h"
#include "game_service.h"
#include "game_server.h"
#include "game_config.h"
#include "global.h"

void core_handle(int)
{
    backTrace();
}

void initDumpCore()
{
    struct sigaction act;
    act.sa_handler = core_handle;
    sigemptyset(&act.sa_mask);
    act.sa_flags = 0;

#ifdef SA_RESTART
    act.sa_flags |= SA_RESTART;         // restart invoken broken by signal
#endif

    sigaction(SIGSEGV, &act, NULL);
    sigaction(SIGFPE, &act, NULL);
    sigaction(SIGBUS, &act, NULL);
}

void server_handle(int signal)
{
    if (!Global::game_server)
    {
        return;
    }
    if (SIGINT == signal || SIGTERM == signal)
    {
        Global::game_server->onRecvShutdownSignal();
    }
    else if (SIGHUP == signal)
    {
        Global::game_server->onRecvReloadSignal();
    }
}

void initServerSignal()
{
    struct sigaction act;
    act.sa_handler = server_handle;
    sigemptyset(&act.sa_mask);
    act.sa_flags = 0;
    sigaction(SIGINT, &act, NULL);
    sigaction(SIGTERM, &act, NULL);
    sigaction(SIGHUP, &act, NULL);
}

int main(int argc, char* argv[])
{
    if (argc != 3)
    {
        std::cout << "Usage: gameserver game_config_file game_name\n";
        return 0;
    }

    initDumpCore();
    initServerSignal();

    // TODO...日志初始化

    // read config from json file
    GameConfig game_config;
    if (!game_config.parseJsonFile(argv[1]))
    {
        ERROR_LOG << "parseJsonFile " << argv[1] << " failed";
        return 0;
    }
    auto iter = game_config.server_configs.find(argv[2]);
    if (iter == game_config.server_configs.end())
    {
        ERROR_LOG << "not found game " << argv[2] << " in config file";
        return 0;
    }
    const auto& server_config = iter->second;

    auto server = std::make_shared<GameServer>(game_config, server_config);
    Global::game_server = server.get();
    server->start();
    server->run();

    return 0;
}

/* vim: set ts=4 sw=4 sts=4 tw=100 */
