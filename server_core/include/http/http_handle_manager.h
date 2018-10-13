/***************************************************************************
 * 
 * Copyright (c) 2018 hpghy, Inc. All Rights Reserved
 * $Id$ 
 * 
 **************************************************************************/
 
 /**
 * @file http_handle_manager.h
 * @author hpghy(790042744@qq.com)
 * @date 2018/07/29 21:10:43
 * @version $Revision$ 
 * @brief 
 *  
 **/
#ifndef HTTP_HANDLE_MANAGER_H
#define HTTP_HANDLE_MANAGER_H

#ifdef __APPLE__
#include <string>
#endif

#include <cstdint>
#include <memory>
#include <unordered_map>
#include <boost/lockfree/queue.hpp>
#include <boost/container/detail/singleton.hpp>

class HttpRequest;
class HttpResponse;

class HttpHandleManager
{
public:
    using RequestHandle = std::function<void(const HttpRequest& req, HttpResponse& rsp)>;

    // singleton_default里面需要调用XXXManager构造函数
    // 这样子XXXManagerInst才能无缝地替换XXXManager, 且防止外界创建新的实例
    friend boost::container::container_detail::singleton_default<HttpHandleManager>;

    template <typename CallbackHandle>
    void addHandle(const std::string& path, CallbackHandle handle)
    {
        if (handle_map_.end() != handle_map_.find(path))
        {
            return;
        }
        handle_map_[path] = handle;
    }
    void delHandle(const std::string& path);

    uint32_t processRequest(uint32_t max_reqs);
    void pushRequest(HttpRequest* req_ptr);
    void processRequest(const HttpRequest& http_req);
    ~HttpHandleManager();

private:
    HttpHandleManager();

private:
    // lockfree::queue<T> T must has trivial con/destructor, std::shared_ptr not suitable
    boost::lockfree::queue<HttpRequest*> request_que_;
    std::unordered_map<std::string, RequestHandle> handle_map_;
};
using HttpHandleManagerInst = boost::container::container_detail::singleton_default<HttpHandleManager>;

#endif  // HTTP_HANDLE_MANAGER_H

/* vim: set ts=4 sw=4 sts=4 tw=100 */
