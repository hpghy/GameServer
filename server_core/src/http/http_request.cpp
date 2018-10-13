/***************************************************************************
 * 
 * Copyright (c) 2018 hpghy, Inc. All Rights Reserved
 * $Id$ 
 * 
 **************************************************************************/
 
 /**
 * @file http_request.cpp
 * @author hpghy(790042744@qq.com)
 * @date 2018/07/29 21:22:29
 * @version $Revision$ 
 * @brief 
 *  
 **/

#include <boost/lexical_cast.hpp>
#include "http/http_request.h"

static std::string CONTENT_TYPE("Content-Length");

bool HttpRequest::parseLine()
{
    // http1.1 has defined 7 method: GET/POST/HEADER/OPTIONS/PUT/DELETE/TRACE
    // TODO...for attention: urlencode....
    std::size_t pos = std::string::npos;
    if ("GET" == method || (method == "POST" && content_type == "application/x-www-form-urlencoded"))
    {
        pos = uri.find('?');
    }

    if (std::string::npos == pos)
    {
        pos = uri.length();
    }
    else if (!parseArgs(uri, pos+1))
    {
        return false;
    }
    path = uri.substr(0, pos);

    // http version, using regex
    if (version.length() == 8)
    {
        major_version = version[5] - '0';
        sub_version = version[7] - '0';
    }

    return true;
}

bool HttpRequest::parseArgs(const std::string& args, std::size_t begin)
{
    std::size_t key_begin = begin;
    std::size_t key_end = 0;
    std::size_t value_begin = 0;
    std::size_t value_end = 0;
    for (auto idx = begin; idx < args.length(); ++idx)
    {
        if ('=' == args[idx])
        {
            key_end = idx;
            value_begin = idx + 1;
        }
        else if ('&' == args[idx])
        {
            value_end = idx;
            parseOneArg(args, key_begin, key_end, value_begin, value_end);

            key_begin = idx + 1;
            key_end = 0;
            value_begin = 0;
            value_end = 0;
        }
    }
    value_end = args.length();
    parseOneArg(args, key_begin, key_end, value_begin, value_end);

    return true;
}

void HttpRequest::parseOneArg(const std::string& args, uint32_t key_begin, uint32_t key_end, uint32_t value_begin, uint32_t value_end)
{
    if (key_begin < key_end && key_end < value_begin && value_begin < value_end)
    {
        args_map.insert(std::make_pair(std::string(args, key_begin, key_end),
                                       std::string(args, value_begin, value_end)));
    }
}

bool HttpRequest::parseHeader()
{
    // parse content_length
    auto iter = header_map.find(CONTENT_TYPE);
    if (iter == header_map.end())
    {
        return false;
    }
    content_length = boost::lexical_cast<std::size_t>(iter->second);

    return true;
}

bool HttpRequest::parseContent()
{
    if ("POST")
        return true;
}

/* vim: set ts=4 sw=4 sts=4 tw=100 */
