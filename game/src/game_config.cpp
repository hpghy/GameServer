/***************************************************************************
 *
 * Copyright (c) 2018, Inc. All Rights Reserved
 * $Id$
 *
 **************************************************************************/

/**
* @file game_config.cpp
* @author hpghy(790042744@qq.com)
* @date 2018/07/01 21:23:42
* @version $Revision$
* @brief
*
**/

#include <exception>
#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/json_parser.hpp>
#include "game_config.h"
#include "util/util.h"

bool GameConfig::parseJsonFile(const char* file_path)
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

void GameConfig::doParseJsonFile(const char* file_path)
{
    boost::property_tree::ptree root;
    boost::property_tree::read_json<boost::property_tree::ptree>(file_path, root);

    //boost::property_tree::ptree common = root.get_child("common");
    for (const auto& server_pt_pair : root)
    {
        if ("common" == server_pt_pair.first)
        {
            continue;
        }
        ServerConfig config;
        config.name = server_pt_pair.first;
        const auto& server_pt = server_pt_pair.second;
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
