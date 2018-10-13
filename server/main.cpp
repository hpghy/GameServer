#include <iostream>
#include <memory>
#include "mobile_server.h"
#include "../service/game_service.h"

#include <boost/log/core.hpp>
#include <boost/log/trivial.hpp>
#include <boost/log/expressions.hpp>
#include <boost/log/sinks/text_file_backend.hpp>
#include <boost/log/utility/setup/file.hpp>
#include <boost/log/utility/setup/common_attributes.hpp>
#include <boost/log/sources/severity_logger.hpp>
#include <boost/log/sources/record_ostream.hpp>

namespace keywords = boost::log::keywords;

void sig_handle(int)
{
    backTrace();
}

void initDumpCore()
{
    struct sigaction act;
    act.sa_handler = sig_handle;
    sigemptyset(&act.sa_mask);
    act.sa_flags = 0;

#ifdef SA_RESTART
    act.sa_flags |= SA_RESTART;         // restart invoken broken by signal
#endif

    sigaction(SIGSEGV, &act, NULL);
    sigaction(SIGFPE, &act, NULL);
    sigaction(SIGBUS, &act, NULL);
}

void initLog()
{
    boost::log::add_file_log
    (
        keywords::file_name = "../log/server_%N.log",
        keywords::rotation_size = 10 * 1024 * 1024,
        keywords::time_based_rotation = boost::log::sinks::file::rotation_at_time_point(0, 0, 0),
        keywords::auto_flush = true,
        keywords::format = "[%TimeStamp%]:%Message%"
    );
    boost::log::add_common_attributes();
}

int main()
{
    initDumpCore();
    initLog();

    ServiceFactoryPtr factory = std::make_shared<GameServiceFactory>();
    std::shared_ptr<MobileServer> server = std::make_shared<MobileServer>(6666, 8080, factory);
    server->run();

    return 0;
}
