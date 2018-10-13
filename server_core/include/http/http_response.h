/***************************************************************************
 * 
 * Copyright (c) 2018 hpghy, Inc. All Rights Reserved
 * $Id$ 
 * 
 **************************************************************************/
 
 /**
 * @file http_response.h
 * @author hpghy(790042744@qq.com)
 * @date 2018/07/29 21:13:49
 * @version $Revision$ 
 * @brief 
 *  
 **/
#ifndef HTTP_RESPONSE_H
#define HTTP_RESPONSE_H

#include <memory>
#include <string>
#include <unordered_map>

class HttpResponse
{
public:
    HttpResponse(): major_version(0), sub_version(0), status_code(202), status("OK")
    {}

    void reset();
    inline void setVersion(uint32_t major, uint32_t sub)
    {
        major_version = major;
        sub_version = sub;
    }

    void setHeader(const std::string& header, const std::string& value);
    void setContent(const std::string& content);
    void setJsonUtf8Content(const std::string&);
    std::shared_ptr<std::string> unparse();

private:
    uint32_t major_version;
    uint32_t sub_version;
    std::unordered_map<std::string, std::string> header_map;
    std::string content;

public:
    uint32_t status_code;
    std::string status;
};

#endif  // HTTP_RESPONSE_H

/* vim: set ts=4 sw=4 sts=4 tw=100 */
