#ifndef KCP_SERVER_HPP_
#define KCP_SERVER_HPP_

#include <unordered_map>
#include <memory>

#include <boost/asio.hpp>
#include "server.h"
#include "../util/util.h"
#include "../connection/kcp_connection.h"


class KcpServer: public Server
{
public:
    explicit KcpServer(boost::asio::io_service&);
    ~KcpServer();

    void bind(const std::string& ip, uint32_t port);
    void listen();
    void start();
    void stop();

    using KcpConnWptr = std::weak_ptr<KcpConnection>;

private:
    void asyncReceive();
    void handleReceive(const boost::system::error_code&, std::size_t);
    void handleSyncRequest();
    void handleAckRequest();
    void setOption();

private:
    udp_addr     bind_addr_;
    udp_addr     rmt_addr_;
    udp_sock     socket_;
    std::array<char, 2048> recv_buffer_{{0}};
};

using KcpServerPtr = std::shared_ptr<KcpServer>;


#endif
