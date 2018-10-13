/***************************************************************************
 *
 * Copyright (c) 2018 hpghy, Inc. All Rights Reserved
 * $Id$
 *
 **************************************************************************/

/**
* @file global.h
* @author hpghy(790042744@qq.com)
* @date 2018/08/12 10:42:52
* @version $Revision$
* @brief
*
**/
#ifndef GLOBAL_H
#define GLOBAL_H

#include <memory>

class GameServer;

class Global
{
    public:
        // 注意shared_ptr绝对不能作为static，因为main结束才会析构，导致资源不能正常释放
        // 按照google c++ style guide，使用原始指针作为static
        static GameServer* game_server;
};

#endif  // GLOBAL_H

/* vim: set ts=4 sw=4 sts=4 tw=100 */
