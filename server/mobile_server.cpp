#ifdef __APPLE__
#define BOOST_LOG_DYN_LINK 1
#endif 

#include <sstream>
#include "mobile_server.h"
#include "../service/game_service.h"
#include "../service/service_manager.h"
#include "../http/http_handle_manager.h"
#include "../util/timer_manager.h"
#include "tcp_server.h"
#include "kcp_server.h"
#include "http_server.h"
#include "../http/http_request.h"
#include "../http/http_response.h"

MobileServer::MobileServer(uint32_t port, uint32_t http_port, ServiceFactoryPtr factory)
    : io_service_(),
      port_(port),
      http_port_(http_port),
      factory_(factory),
      kcp_server_(std::make_shared<KcpServer>(io_service_)),
      tcp_server_(std::make_shared<TcpServer>(io_service_)),
      http_server_(std::make_shared<HttpServer>(io_service_))
{
    INFO_LOG << "create MobileServer";
    ServiceManager::instance().setServiceFactory(factory);
}

MobileServer::~MobileServer()
{
    unsetWork();
    kcp_server_.reset();
    tcp_server_.reset();
    http_server_.reset();
}

void MobileServer::run()
{
    if (running_)
    {
        return;
    }

    INFO_LOG << "main thread: " << std::this_thread::get_id() << " start work!";
    running_ = true;

    initBeforeStartServer();

    kcp_server_->bind("127.0.0.1", port_);
    kcp_server_->listen();
    kcp_server_->start();

    tcp_server_->bind("127.0.0.1", port_);
    tcp_server_->listen();
    tcp_server_->start();

    http_server_->bind("127.0.0.1", http_port_);
    http_server_->listen();
    http_server_->start();

    initBeforeStartThread();

    setWork();

    // 创建IO子线程
    worker_join_.reset(new std::thread(std::bind(&MobileServer::createWorkers,
                                       shared_from_this())));

    //添加信号处理函数, TODO... move to main function
    boost::asio::signal_set signals(io_service_);
    signals.add(SIGINT);
    signals.add(SIGTERM);
#ifdef SIGQUIT
    signals.add(SIGQUIT);
#endif
    //注意子线程执行
    signals.async_wait(std::bind(&MobileServer::sigInitHandle, this));

    //SIGUSER信号
    boost::asio::signal_set signals2(io_service_, SIGUSR1);
    signals2.async_wait(std::bind(&MobileServer::sigUserHandle, this));

    while (1)
    {
        if (!running_)
            break;
        poll();
    }
    stop();
}

void MobileServer::initBeforeStartServer()
{
}

void MobileServer::initBeforeStartThread()
{
    // test http handle manager
    auto handle = std::bind(&MobileServer::printHttpRequest, shared_from_this(), std::placeholders::_1, std::placeholders::_2);
    HttpHandleManager::instance().addHandle("/hptest", handle);

    // test TimerManager
    TimerManager::instance().addTimer(10, std::bind(&MobileServer::printTimer, shared_from_this()));
}

void MobileServer::sigInitHandle()
{
    INFO_LOG << "receive int signal";
    running_ = false;
}

void MobileServer::sigUserHandle()
{
    INFO_LOG << "receive user1 signal";
}

void MobileServer::printHttpRequest(const HttpRequest& request, HttpResponse& response)
{
    DEBUG_LOG << "print HttpRequest for path: " << request.getPath();
    response.status_code = 202;
    response.status = "EchoTest";

    std::ostringstream oss;
    const auto& args_map = request.getArgsMap();
    for (auto iter = args_map.cbegin(); iter != args_map.cend(); ++iter)
    {
        oss << iter->first << "=" << iter->second << "&";
    }
    oss << "path: " << request.getPath();

    response.setContent(oss.str());
}

void MobileServer::printTimer()
{
    DEBUG_LOG << "in MobileServer::printTimer";
    TimerManager::instance().addTimer(30, std::bind(&MobileServer::printTimer, shared_from_this()));
}

void MobileServer::stop()
{
    INFO_LOG << "MobileServer::stop()";

    kcp_server_->stop();
    tcp_server_->stop();
    http_server_->stop();

    unsetWork();
    io_service_.stop();
    if (worker_join_)
    {
        worker_join_->join();
    }
}

void MobileServer::createWorkers()
{
    INFO_LOG << "create workers manager: " << std::this_thread::get_id();
    auto self(shared_from_this());
    std::vector<std::thread> threads;
    for (unsigned int i = 0; i < thread_cnt_; ++ i)
    {
        threads.emplace_back([this, self]() {
            io_service_.run();
        });
        INFO_LOG << "create worker: " << threads[i].get_id();
    }

    for (unsigned int i = 0; i < thread_cnt_; ++ i)
    {
        threads[i].join();
    }
    INFO_LOG << "MobileServer workers exit!!!";
}

void MobileServer::poll(bool sleep)
{
    //主线程使用
    if (running_)
    {
        auto start = stamp_microsecond();

        //处理rpc
        auto cnt = ServiceManager::instance().processRequest(100);
        if (cnt > 50)
        {
            INFO_LOG << "processRequest " << cnt;
        }

        HttpHandleManager::instance().processRequest(50);

        TimerManager::instance().tick(50);

        //睡眠一定的时间
        auto delta = stamp_microsecond() - start;
        if (sleep && delta < 1000)
        {
            std::this_thread::sleep_for(std::chrono::microseconds(5000));
        }
    }
}


