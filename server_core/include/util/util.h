#ifndef KCP_UTIL_H
#define KCP_UTIL_H

// Makefile -D BOOST_LOG_DYN_LINK 无效
#ifdef __APPLE__
#define BOOST_LOG_DYN_LINK 1
#endif

#include <chrono>
#include <memory>
#include <thread>
#include <boost/asio.hpp>
#include <boost/log/core.hpp>
#include <boost/log/trivial.hpp>
#include <boost/log/expressions.hpp>

#ifdef DEBUG
#include <iostream>
#endif
#include <execinfo.h>

#ifdef _WIN32

#else
typedef int32_t				int32;
typedef unsigned int		uint32;
typedef int64_t				int64;
typedef uint64_t			uint64;
#endif

#define KCP_OVERHEAD 	24

#define THREAD_ID std::this_thread::get_id()
#define DEBUG_LOG	BOOST_LOG_TRIVIAL(debug) << THREAD_ID << " " << __FILE__ << " " << __LINE__ << " DEBUG" << ">>"
#define INFO_LOG	BOOST_LOG_TRIVIAL(info) << THREAD_ID << " " << __FILE__ << " " << __LINE__ << " INFO" << ">>"
#define WARN_LOG	BOOST_LOG_TRIVIAL(warning) << THREAD_ID << " " << __FILE__ << " " << __LINE__ << " WARNING" << ">>"
#define ERROR_LOG	BOOST_LOG_TRIVIAL(error) << THREAD_ID << " " << __FILE__ << " " << __LINE__ << " ERROR" << ">>"
#define FATAL_LOG	BOOST_LOG_TRIVIAL(fatal) << THREAD_ID << " " << __FILE__ << " " << __LINE__ << " FATAL" << ">>"

/**
#define DEBUG_LOG	std::cout
#define INFO_LOG	std::cout
#define WARN_LOG	std::cout
#define ERROR_LOG	std::cout
#define FATAL_LOG	std::cout
**/

using udp_addr = boost::asio::ip::udp::endpoint;
using udp_sock = boost::asio::ip::udp::socket;
using udp_space = boost::asio::ip::udp;
using tcp_addr = boost::asio::ip::tcp::endpoint;
using tcp_sock = boost::asio::ip::tcp::socket;
using tcp_space = boost::asio::ip::tcp;
using boost_err = boost::system::error_code;
using write_buf_ptr = std::shared_ptr<boost::asio::streambuf>;

#define STRAND_POST(strand, class, func, ...)		\
		strand->post(std::bind(&class::func, \
					std::static_pointer_cast<class>(std::shared_from_this()),	\
					##__VA_ARGS__					\
					))

/**
 微妙时间戳
 **/
inline uint64 stamp_microsecond()
{
    return std::chrono::duration_cast<std::chrono::microseconds>
           (std::chrono::system_clock::now().time_since_epoch()).count();
}

/**
 毫妙时间戳
 **/
inline uint64 stamp_millisecond()
{
    return std::chrono::duration_cast<std::chrono::milliseconds>
           (std::chrono::system_clock::now().time_since_epoch()).count();
}

void backTrace();


#endif
