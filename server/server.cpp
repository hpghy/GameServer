#include "server.h"

Server::Server(boost::asio::io_service& io_service)
    : io_service_(io_service)
{
}

void Server::bind(const std::string& ip, uint32_t port)
{
    host_ = ip;
    port_ = port;
}
