/***************************************************************************
 * 
 * Copyright (c) 2018 hpghy, Inc. All Rights Reserved
 * $Id$ 
 * 
 **************************************************************************/
 
 /**
 * @file gate_config.cpp
 * @author hpghy(790042744@qq.com)
 * @date 2018/07/01 21:43:13
 * @version $Revision$ 
 * @brief 
 *  
 **/

#include <exception>
#include "gate_config.h"
#include "util/util.h"

bool GateConfig::parseJsonFile(const char* file_path)
{
    try
    {
        doParseJsonFile(file_path);
    }
    catch (std::exception& e)
    {
        ERROR_LOG << "doParseJsonFile throw exception: " << e.what();
        return false;
    }
    return true;
}

void GateConfig::doParseJsonFile(const char* file_path)
{
    boost::property_tree::ptree root;
    boost::property_tree::read_json<boost::property_tree::ptree>(file_path, root);

    boost::property_tree::ptree common = root.get_child("common");
    kcp_enabled = common.get<bool>("kcp_enabled", false);
    compress_enabled = common.get<bool>("compress_enabled", false);
    encryption_enabled = common.get<bool>("encryption_enabled", false);

    for (const auto& server_pt_it : root)
    {
        if ("common" == server_pt_it.first)
        {
            continue;
        }
        ServerConfig config;
        config.name = server_pt_it.first;
        const auto& server_pt = server_pt_it.second;
        config.host = server_pt.get<std::string>("host", "127.0.0.1");
        config.port = server_pt.get<uint16_t>("port");
        config.http_host = server_pt.get<std::string>("http_host", "127.0.0.1");
        config.http_port = server_pt.get<uint16_t>("http_port", 0);
        if (!server_configs.insert(std::make_pair(config.name, config)).second)
        {
            ERROR_LOG << "duplicate server name: " << config.name;
        }
    }
}

/* vim: set ts=4 sw=4 sts=4 tw=100 */
