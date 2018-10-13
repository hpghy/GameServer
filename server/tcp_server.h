#ifndef TCP_SERVER_HPP_
#define TCP_SERVER_HPP_

#include <memory>
#include <boost/asio.hpp>
#include "server.h"
#include "../util/util.h"

class TcpConnection;
using TcpConnectionPtr = std::shared_ptr<TcpConnection>;

class ServiceFactory;
using ServiceFactoryPtr = std::shared_ptr<ServiceFactory>;

class TcpServer: public Server
{
public:
    explicit TcpServer(boost::asio::io_service&);
    ~TcpServer();

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
    TcpConnectionPtr         conn_ptr_;
    uint32_t                 conns_;
};

using TcpServerPtr = std::shared_ptr<TcpServer>;

#endif
