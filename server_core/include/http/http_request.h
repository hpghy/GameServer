/***************************************************************************
 * 
 * Copyright (c) 2018 hpghy, Inc. All Rights Reserved
 * $Id$ 
 * 
 **************************************************************************/
 
 /**
 * @file http_request.h
 * @author hpghy(790042744@qq.com)
 * @date 2018/07/29 21:14:52
 * @version $Revision$ 
 * @brief 
 *  
 **/
#ifndef HTTP_REQUEST_H
#define HTTP_REQUEST_H

#include <string>
#include <cstdint>
#include <unordered_map>
#include "../util/util.h"

class HttpRequest
{
public:
    using HttpReqMap = std::unordered_map<std::string, std::string>;

public:
    HttpRequest(): major_version(0), sub_version(0), content_length(0)
    {
        DEBUG_LOG << "Create HttpRequest version: " << version;
    }

    inline void reset()
    {
        major_version = sub_version = content_length = 0;
        method.clear();
        uri.clear();
        version.clear();
        header_map.clear();
        args_map.clear();
        content_type.clear();
        content.clear();
    }

    inline uint32_t getMajorVersion() const { return major_version; }
    inline uint32_t getSubVersion() const { return sub_version; }
    inline std::size_t getContentLength() const { return content_length; }
    inline const std::string& getPath() const { return path; }
    const HttpReqMap& getArgsMap() const { return args_map; }

    inline bool addHeader(const std::string& name, const std::string& value)
    {
        return header_map.insert(std::make_pair(name, value)).second;
    }

    bool parseLine();
    bool parseArgs(const std::string& args, std::size_t begin);
    void parseOneArg(const std::string& args, uint32_t key_begin, uint32_t key_end, uint32_t value_begin, uint32_t value_end);
    bool parseHeader();
    bool parseContent();

private:
    uint32_t major_version;
    uint32_t sub_version;
    std::size_t content_length;
    std::string path;
    std::string content_type;
    HttpReqMap args_map;

public:
    std::string method;
    std::string uri;
    std::string version;
    HttpReqMap header_map;
    std::string content;
};

#endif  // HTTP_REQUEST_H

/* vim: set ts=4 sw=4 sts=4 tw=100 */
