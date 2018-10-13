/***************************************************************************
 *
 * Copyright (c) 2018 hpghy, Inc. All Rights Reserved
 * $Id$
 *
 **************************************************************************/

/**
* @file main.cpp
* @author hpghy(790042744@qq.com)
* @date 2018/07/15 17:44:29
* @version $Revision$
* @brief
*
**/

#include <iostream>
#include <memory>
#include "mobile_client.h"
#include "util/util.h"

void sig_handle(int)
{
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

int main(int argc, char* argv[])
{
    if (4 != argc)
    {
        ERROR_LOG << "client ip port\n";
        return 0;
    }
    std::string ip = argv[1];
    uint16_t port = atoi(argv[2]);
    std::string param = argv[3];
    if (0 == port)
    {
        ERROR_LOG << "port is invalid";
        return 0;
    }

    initDumpCore();

    boost::asio::io_service io_service;
    std::shared_ptr<MobileClient> client = std::make_shared<MobileClient>(io_service, ip, port);
    client->param = param;
    client->start();
    client->run();

    return 0;
}


/* vim: set ts=4 sw=4 sts=4 tw=100 */
