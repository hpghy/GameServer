/***************************************************************************
 *
 * Copyright (c) 2018, Inc. All Rights Reserved
 * $Id$
 *
 **************************************************************************/

/**
* @file gate_config.h
* @author hpghy(790042744@qq.com)
* @date 2018/07/01 21:38:50
* @version $Revision$
* @brief
*
**/
#ifndef GATE_CONFIG_H
#define GATE_CONFIG_H

#include <cstdint>
#include <string>
#include <unordered_map>
#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/json_parser.hpp>

struct DeviceClient
{
    std::string addr;
    std::string deviceid;
    std::string game_name;
    uint32_t session_id;
};

struct ServerConfig
{
    std::string name;
    std::string host;
    std::uint16_t port;
    std::string http_host;
    std::uint16_t http_port;
};

struct GateConfig
{
    GateConfig(): kcp_enabled(false), compress_enabled(false), encryption_enabled(false)
    {}

    bool parseJsonFile(const char* file_path);
    void doParseJsonFile(const char* file_path);

    //common 块
    bool kcp_enabled;
    bool compress_enabled;
    bool encryption_enabled;

    // 各组serverconfig
    std::unordered_map<std::string, ServerConfig> server_configs;
};

#endif  // GATE_CONFIG_H

/* vim: set ts=4 sw=4 sts=4 tw=100 */
