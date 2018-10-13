#ifndef HTTP_SERVER_HPP_
#define HTTP_SERVER_HPP_

#include "tcp_server.h"
#include "../util/util.h"

class HttpConnection;
using HttpConnectionPtr = std::shared_ptr<HttpConnection>;

class HttpServer: public Server
{
public:
    HttpServer(boost::asio::io_service&);
    ~HttpServer();

    void bind(const std::string& ip, uint32_t port);
    void listen();
    void start();
    void stop();

private:
    void setOption();
    void asyncReceive();
    void handleAccept(const boost_err&);

private:
    boost::asio::ip::tcp::acceptor acceptor_;
    HttpConnectionPtr           conn_ptr_;
};

using HttpServerPtr = std::shared_ptr<HttpServer>;

#endif
