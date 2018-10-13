/***************************************************************************
 * 
 * Copyright (c) 2018, Inc. All Rights Reserved
 * $Id$ 
 * 
 **************************************************************************/
 
 /**
 * @file game_config.h
 * @author hpghy(790042744@qq.com)
 * @date 2018/07/01 21:16:35
 * @version $Revision$ 
 * @brief 
 *  
 **/
#ifndef GAME_CONFIG_H
#define GAME_CONFIG_H

#include <cstdint>
#include <string>
#include <unordered_map>
using namespace std;

struct ServerConfig
{
    std::string name;
    std::string host;
    std::uint16_t port;
    std::string http_host;
    std::uint16_t http_port;
};

struct GameConfig
{
    bool parseJsonFile(const char* file_path);
    void doParseJsonFile(const char* file_path);

    // common
    
    std::unordered_map<std::string, ServerConfig> server_configs;
};

#endif  // GAME_CONFIG_H

/* vim: set ts=4 sw=4 sts=4 tw=100 */
