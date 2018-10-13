/***************************************************************************
 *
 * Copyright (c) 2018 hpghy, Inc. All Rights Reserved
 * $Id$
 *
 **************************************************************************/

/**
* @file main.cpp
* @author hpghy(790042744@qq.com)
* @date 2018/07/01 21:45:49
* @version $Revision$
* @brief
*
**/

#include <iostream>
#include <memory>
#include "util/util.h"
#include "gate_service.h"
#include "gate_server.h"
#include "gate_config.h"
#include "global.h"

void sig_handle(int signal)
{
    std::cerr << "sig_handle " << signal << " and then invoke backTrace";
    backTrace();
}

void initDumpCore()
{
    struct sigaction act;
    act.sa_handler = sig_handle;
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
    if (!Global::gate_server)
    {
        return;
    }
    if (SIGINT == signal || SIGTERM == signal)
    {
        Global::gate_server->onRecvShutdownSignal();
    }
    else if (SIGHUP == signal)
    {
        Global::gate_server->onRecvReloadSignal();
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
        std::cout << "Usage: gateserver gate_config_file server_name\n";
        return 0;
    }

    initDumpCore();
    initServerSignal();

    // read config from json file
    GateConfig gate_config;
    if (!gate_config.parseJsonFile(argv[1]))
    {
        ERROR_LOG << "parseJsonFile " << argv[1] << " failed";
        return 0;
    }
    auto iter = gate_config.server_configs.find(argv[2]);
    if (iter == gate_config.server_configs.end())
    {
        ERROR_LOG << "not found server: " << argv[2] << " in " << argv[1];
        return 0;
    }
    const auto& self_config = iter->second;

    // TODO...日志初始化

    std::shared_ptr<GateServer> server = std::make_shared<GateServer>(gate_config, self_config);
    Global::gate_server = server.get();
    server->start();
    server->run();

    return 0;
}

/* vim: set ts=4 sw=4 sts=4 tw=100 */
