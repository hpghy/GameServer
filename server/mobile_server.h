#ifndef MOBILE_SERVER_HPP_
#define MOBILE_SERVER_HPP_

#include <memory>
#include <thread>
#include <boost/asio.hpp>
#include <boost/noncopyable.hpp>

#include "../util/util.h"
#include "../service/rpc_service.h"

class TcpServer;
using TcpServerPtr = std::shared_ptr<TcpServer>;
class KcpServer;
using KcpServerPtr = std::shared_ptr<KcpServer>;
class HttpServer;
using HttpServerPtr = std::shared_ptr<HttpServer>;

class HttpRequest;
class HttpResponse;

class MobileServer: public boost::noncopyable,
                    public std::enable_shared_from_this<MobileServer>
{
public:
    MobileServer(uint32_t port, uint32_t http_port, ServiceFactoryPtr factory);
    ~MobileServer();

	void run();
	void stop();
	void poll(bool sleep = true);

	void setWork()
	{
		//保证io_service::run不会退出
		work_.reset(new boost::asio::io_service::work(io_service_));
	}

	void unsetWork()
	{
		work_.reset();
	}

	inline void setThreadCount(unsigned int cnt)
	{
		if (cnt > 0)
		{
			thread_cnt_ = cnt;
		}
	}

	inline unsigned int getThreadCount()
	{
		return thread_cnt_;
	}

private:
	void createWorkers();
	void sigInitHandle();
	void sigUserHandle();
    void initBeforeStartServer();
    void initBeforeStartThread();

// test codes...
private:
    void printHttpRequest(const HttpRequest& request, HttpResponse& response);
    void printTimer();

private:
	boost::asio::io_service io_service_;
	unsigned int 	port_;
    unsigned int    http_port_;
	unsigned int 	thread_cnt_ = 2;

    ServiceFactoryPtr factory_;
	KcpServerPtr  kcp_server_;
	TcpServerPtr  tcp_server_;
    HttpServerPtr http_server_;
	std::shared_ptr<boost::asio::io_service::work> work_;
	std::shared_ptr<std::thread> worker_join_;
	bool			running_ = false;
};


#endif
