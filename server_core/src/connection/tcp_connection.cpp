/***************************************************************************
 *
 * Copyright (c) 2018 hpghy, Inc. All Rights Reserved
 * $Id$
 *
 **************************************************************************/

/**
* @file tcp_connection.cpp
* @author hpghy(790042744@qq.com)
* @date 2018/07/29 14:21:02
* @version $Revision$
* @brief
*
**/

#include "connection/tcp_connection.h"
#include "connection/rpc_channel.h"
#include "service/request_queue.h"
#include "util/util.h"

TcpConnection::TcpConnection(boost::asio::io_service& io_service, std::weak_ptr<Server> server_wptr)
    : Connection(io_service, server_wptr),
      socket_(io_service),
      recv_buffer_(2048)
{
}

TcpConnection::~TcpConnection()
{
    //有可能socket_已经关闭了，不能使用remote_endpoint()
    DEBUG_LOG << "destroy TcpConnection: " << getRemoteAddr();
}

// io线程执行
void TcpConnection::doStartWork()
{
    setOption();
    DEBUG_LOG << "TcpConnection " << getRemoteAddr() << " non_blocking: " << socket_.non_blocking();
    asyncReceive();
}

void TcpConnection::setOption()
{
    boost::system::error_code ec;
    socket_.set_option(boost::asio::ip::tcp::socket::keep_alive(true), ec);
    if (ec)
    {
        ERROR_LOG << "set_option keep_alive(true) failel: " << ec.message();
    }
    socket_.set_option(boost::asio::ip::tcp::no_delay(true), ec);
    if (ec)
    {
        ERROR_LOG << "set_option no_delay(true) failel: " << ec.message();
    }
    socket_.non_blocking(true, ec);
    if (ec)
    {
        ERROR_LOG << "non_blocking failed: " << ec.message();
    }
    //TODO...设置socket发送接收缓存区大小
}

void TcpConnection::asyncReceive()
{
    auto self = std::dynamic_pointer_cast<TcpConnection>(shared_from_this());
    auto handle = [self](const boost_err & ec, std::size_t length) { self->handleReceive(ec, length); };
    socket_.async_receive(boost::asio::buffer(recv_buffer_), getRecvStrand()->wrap(handle));
}

// io线程中使用recv_strand保护
void TcpConnection::handleReceive(const boost_err& ec, std::size_t length)
{
    if (length == 0)
    {
        INFO_LOG << "Read zero bytes and close socket!!!";
        closeSocket();
        return;
    }
    if (ec)
    {
        ERROR_LOG << "handle_receive failed: " << ec.value() << " " << ec.message();
        closeSocket();
        return;
    }

    auto channel_ptr = getChannel().lock();
    if (!channel_ptr)
    {
        ERROR_LOG << "TcpConnection channel_ptr is null";
    }
    else if (!channel_ptr->handleData(recv_buffer_.data(), length))
    {
        ERROR_LOG << "TcpConnection handle_data failed...";
    }
    else
    {
        asyncReceive();
    }
}

void TcpConnection::doAsyncSendData(std::shared_ptr<std::string> data_ptr)
{
    if (isClosed())
    {
        WARN_LOG << "TcpConnection has been disconnected!";
        return;
    }

    //为什么要有sending_标志？
    //1. strand可以保证只有一个线程进入do_async_send_data发起异步写请求，
    //   但很可能两个异步写操作实际写入顺序还是有问题。所以需要保证：
    //   一个异步写操作完成后才能发起下一个异步写请求
    //   有时间看boost asio相关源代码
    data_queue_.push_back(data_ptr);
    if (!is_sending_)
    {
        startAsyncSend();
    }
}

void TcpConnection::startAsyncSend()
{
    if (isClosed())
    {
        WARN_LOG << "socket has been closed";
        return;
    }
    is_sending_ = true;
    sending_queue_.clear();

    // 把缓存中的数据设置成正在异步发送
    data_queue_.swap(sending_queue_);
    std::vector<boost::asio::const_buffer> buffers;
    sending_size_ = 0;
    for (const auto& buffer : sending_queue_)
    {
        buffers.push_back(boost::asio::buffer(buffer->c_str(), buffer->size()));
        sending_size_ += buffer->size();
    }
    DEBUG_LOG << "sending_queue_ size: " << sending_queue_.size() << " bytes: " << sending_size_;

    /*
     * 如果没有没有snd_strand包裹，handleAsyncWrite和doAsyncSendData可以同时执行，data_queue_有冲突,导致程序崩溃
    boost::asio::async_write(socket_, buffers, std::bind(&TcpConnection::handleAsyncWrite,
                             std::static_pointer_cast<TcpConnection>(shared_from_this()),
                             std::placeholders::_1, std::placeholders::_2));
    */
    // 注意和socket::async_send的区别
    auto self = std::dynamic_pointer_cast<TcpConnection>(shared_from_this());
    auto handle = [self](const boost_err & ec, std::size_t bytes) { self->handleAsyncWrite(ec, bytes); };
    boost::asio::async_write(socket_, buffers, getSendStrand()->wrap(handle));
}

// io线程中执行
void TcpConnection::handleAsyncWrite(const boost_err& ec, std::size_t bytes)
{
    if (ec)
    {
        ERROR_LOG << "handleAsyncWrite failed: " << ec.value() << " " << ec.message();
        closeSocket();
        return;
    }
    if (bytes != sending_size_)
    {
        ERROR_LOG << "length: " << bytes << " != sending_size_: " << sending_size_;
    }

    sending_queue_.clear();
    if (data_queue_.empty())
    {
        // 完成异步写操作
        is_sending_ = false;
    }
    else
    {
        // 继续发起异步写操作
        startAsyncSend();
    }
}

// io线程执行
void TcpConnection::handleCloseSocket()
{
    INFO_LOG << getRemoteAddr() << " handleCloseSocket";
    if (socket_.is_open())
    {
        boost_err err;
        socket_.close(err);
        if (err)
        {
            ERROR_LOG << "TcpConnection close socket error " << err.message() \
                      << " value: " << err.value();
        }
    }
}


/* vim: set ts=4 sw=4 sts=4 tw=100 */
