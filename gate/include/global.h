/***************************************************************************
 *
 * Copyright (c) 2018 hpghy, Inc. All Rights Reserved
 * $Id$
 *
 **************************************************************************/

/**
* @file global.h
* @author hpghy(790042744@qq.com)
* @date 2018/08/12 10:28:14
* @version $Revision$
* @brief
*
**/
#ifndef GLOBAL_H
#define GLOBAL_H

#include <memory>

class GateServer;
using GateServerPtr = std::shared_ptr<GateServer>;

class Global
{
    public:
        // std::shared_ptr有non-trivial destructor，不能作为static变量
        static GateServer* gate_server;
};

#endif  // GLOBAL_H

/* vim: set ts=4 sw=4 sts=4 tw=100 */
