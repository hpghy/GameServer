/***************************************************************************
 *
 * Copyright (c) 2018 hpghy, Inc. All Rights Reserved
 * $Id$
 *
 **************************************************************************/

/**
* @file http_connection.cpp
* @author hpghy(790042744@qq.com)
* @date 2018/07/29 21:20:47
* @version $Revision$
* @brief
*
**/

#include "http/http_connection.h"

#include "util/util.h"
#include "http/http_handle_manager.h"
#include "server/server.h"

HttpConnection::HttpConnection(boost::asio::io_service& io_service)
    : TcpConnection(io_service, ServerWptr()),
      req_ptr_(nullptr)
{
    DEBUG_LOG << "Crete HttpConnection ";
}

HttpConnection::~HttpConnection()
{
    DEBUG_LOG << "destroy HttpConnection: " << getRemoteAddr();
}


void HttpConnection::handleReceive(const boost_err& ec, std::size_t length)
{
    DEBUG_LOG << "http read bytes: " << length;
    if (0 == length)
    {
        DEBUG_LOG << "Read zero bytes and close socket!!!";
        closeSocket();
        return;
    }
    if (ec)
    {
        ERROR_LOG << "handle_receive failed: " << ec.value() << " " << ec.message();
        closeSocket();
        return;
    }
    if (nullptr == req_ptr_)
    {
        req_ptr_ = new HttpRequest();
    }

    http_req_parser_.pushData(getRecvBuffer().data(), length);
    while (true)
    {
        auto ret = http_req_parser_.parse(*req_ptr_);
        if (ret)
        {
            HttpHandleManagerInst::instance().pushRequest(req_ptr_);
            //asyncSend(http_rsp_.unparse());
            req_ptr_ = new HttpRequest();
        }
        else
        {
            if (!ret)
            {
                ERROR_LOG << "http request parse error";
                closeSocket();
                return;
            }
            break;
        }
    }
    asyncReceive();
}


/* vim: set ts=4 sw=4 sts=4 tw=100 */
