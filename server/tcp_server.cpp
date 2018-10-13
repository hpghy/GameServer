#ifdef __APPLE__
#define BOOST_LOG_DYN_LINK 1
#endif

#include <boost/lexical_cast.hpp>
#include "tcp_server.h"
#include "../util/util.h"
#include "../connection/tcp_connection.h"
#include "../service/service_manager.h"
#include "../service/rpc_service.h"

TcpServer::TcpServer(boost::asio::io_service& io_service)
    : Server(io_service),
      acceptor_(io_service),
      conns_(0)
{
    INFO_LOG << "Create TcpServer";
}

TcpServer::~TcpServer()
{
    INFO_LOG << "Destroy TcpServer";
}

void TcpServer::bind(const std::string& ip, uint32_t port)
{
    Server::bind(ip, port);

    boost::asio::ip::tcp::resolver resolver(io_service_);
    boost::asio::ip::tcp::resolver::query query(host_, boost::lexical_cast<std::string>(port_));
    boost::asio::ip::tcp::endpoint endpoint = *resolver.resolve(query);

    if (acceptor_.is_open())
        acceptor_.close();

    acceptor_.open(endpoint.protocol());
    setOption();
    acceptor_.bind(endpoint);
}

void TcpServer::setOption()
{
    boost::asio::socket_base::reuse_address option(true);
    acceptor_.set_option(option);
    acceptor_.set_option(boost::asio::ip::tcp::socket::keep_alive(true));
    acceptor_.set_option(boost::asio::ip::tcp::no_delay(true));
    boost::system::error_code ec;
    acceptor_.non_blocking(true, ec);
    if (ec)
    {
        ERROR_LOG << "non_blocking failed: " << ec.message();
    }
}

void TcpServer::listen()
{
    //在nginx、redis、apache里，默认的backlog值是511
    acceptor_.listen(511);
    asyncReceive();
}

void TcpServer::start()
{
    Server::start();
}

void TcpServer::asyncReceive()
{
    // 这里没有必要使用strand因为io_service只会存在一个handler
    conn_ptr_ = std::make_shared<TcpConnection>(io_service_);
    acceptor_.async_accept(conn_ptr_->socket(), std::bind(&TcpServer::handleAccept,
                           std::static_pointer_cast<TcpServer>(shared_from_this()),
                           std::placeholders::_1));
}

void TcpServer::handleAccept(const boost_err& ec)
{
    // 同时只有一个io线程会执行该函数
    if (!ec)
    {
        ++ conns_;
        const auto& rmt_addr = conn_ptr_->socket().remote_endpoint();
        conn_ptr_->setRemoteaddr(rmt_addr);
        INFO_LOG << "new connection " << conns_ << " peer: " << rmt_addr << " addr: " << conn_ptr_->getRemoteAddr();

        // TODO...也可以在ServiceManager对connection集合加锁，没有必要把callback传递给主线程
        // avoid tcp_server has been destoried
        auto conn(conn_ptr_);
        auto self(shared_from_this());
        auto callback = [self, conn](void)
        {
            // execute in main thread, and conn_ptr_ is new object, not old one!
            ServiceManager::instance().onConnected(conn);
            conn->startWork();
        };
        QueueRequest *request = new QueueCallbackRequest(callback);
        ServiceManager::instance().pushRequest(request);

        asyncReceive();
    }
    else {
        ERROR_LOG << "handle_accept failed " << ec.value() << " " << ec.message();
    }
}

void TcpServer::stop()
{
    Server::stop();
    if (acceptor_.is_open())
    {
        boost_err ec;
        acceptor_.close(ec);
        if (ec)
        {
            ERROR_LOG << "TcpServer close socket error " << ec.value()
                      << " " << ec.message();
        }
    }
}
