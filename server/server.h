#ifndef SERVER_HPP_
#define SERVER_HPP_

#include <memory>
#include <cstdint>
#include <boost/asio.hpp>

class Server: private boost::noncopyable,
            public std::enable_shared_from_this<Server>
{
public:
    enum class Status
    {
        SERVER_STATUS_INIT  = 0,
        SERVER_STATUS_START = 1,
        SERVER_STATUS_BUSY  = 2,
        SERVER_STATUS_STOP  = 3,
        SERVER_STATUS_ERROR = 4
    };

public:
    Server(boost::asio::io_service&);
    virtual ~Server() {};

    // bind/listen/start invoked before create threads
    virtual void bind(const std::string& ip, uint32_t port);
    virtual void listen() = 0;
    virtual void start()
    {
        stopped_ = false;
    }

    // invoked in main thread!
    virtual void stop()
    {
        stopped_ = true;
    }

    bool isStopped()
    {
        // not thread safe, but not effect.
        return stopped_;
    }

protected:
    boost::asio::io_service&        io_service_;
    std::string          host_;
    uint32_t             port_;
    bool                 stopped_ = true;
};

using server_ptr = std::shared_ptr<Server>;

#endif
