/***************************************************************************
 * 
 * Copyright (c) 2018 hpghy, Inc. All Rights Reserved
 * $Id$ 
 * 
 **************************************************************************/
 
 /**
 * @file http_handle_manager.cpp
 * @author hpghy(790042744@qq.com)
 * @date 2018/07/29 21:21:28
 * @version $Revision$ 
 * @brief 
 *  
 **/

#include "util/util.h"
#include "http/http_handle_manager.h"
#include "http/http_request.h"
#include "http/http_response.h"

HttpHandleManager::HttpHandleManager()
    : request_que_(100)
{
    INFO_LOG << "Create HttpHandleManager";
}

HttpHandleManager::~HttpHandleManager()
{
    DEBUG_LOG << "Destroy HttpHandleManager";
    HttpRequest* req_ptr = nullptr;
    while (request_que_.pop(req_ptr))
    {
        delete req_ptr;
    }
}

uint32_t HttpHandleManager::processRequest(uint32_t max_num)
{
    uint32_t cnt = 0;
    HttpRequest* req_ptr = nullptr;
    while (request_que_.pop(req_ptr) && nullptr != req_ptr)
    {
        processRequest(*req_ptr);
        delete req_ptr;
        ++cnt;
        if (cnt >= max_num)
        {
            break;
        }
    }
    return cnt;
}

void HttpHandleManager::processRequest(const HttpRequest&)
{
    // invoke in main thread
    /**
    auto iter = handle_map_.find(http_req.getPath());
    if (iter != handle_map_.end())
    {
        iter->second(http_req, http_rsp);
    }
    else
    {
        http_rsp.status_code = 404;
        http_rsp.status.assign("Not found http path handle");
    }
    **/
}

void HttpHandleManager::pushRequest(HttpRequest* req_ptr)
{
    request_que_.push(req_ptr);
}

/* vim: set ts=4 sw=4 sts=4 tw=100 */
