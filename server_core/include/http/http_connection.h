/***************************************************************************
 * 
 * Copyright (c) 2018 hpghy, Inc. All Rights Reserved
 * $Id$ 
 * 
 **************************************************************************/
 
 /**
 * @file http_connection.h
 * @author hpghy(790042744@qq.com)
 * @date 2018/07/29 21:10:01
 * @version $Revision$ 
 * @brief 
 *  
 **/
#ifndef HTTP_CONNECTION_H
#define HTTP_CONNECTION_H

#include "../connection/tcp_connection.h"
#include "http_request.h"
#include "http_request_parser.h"
#include "http_response.h"
#include "../util/util.h"

class HttpConnection: public TcpConnection
{
public:
    HttpConnection(boost::asio::io_service& io_service);
    ~HttpConnection();

	virtual void handleReceive(const boost_err&, std::size_t);

private:
    HttpRequest*        req_ptr_;
    HttpRequestParser   http_req_parser_;
    HttpResponse        http_rsp_;
};

#endif  // HTTP_CONNECTION_H

/* vim: set ts=4 sw=4 sts=4 tw=100 */
