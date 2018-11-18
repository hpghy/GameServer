/***************************************************************************
 *
 * Copyright (c) 2018 hpghy, Inc. All Rights Reserved
 * $Id$
 *
 **************************************************************************/

/**
* @file kcp_connection.h
* @author hpghy(790042744@qq.com)
* @date 2018/07/29 14:07:19
* @version $Revision$
* @brief
*
**/
#ifndef KCP_CONNECTION_H
#define KCP_CONNECTION_H

#include <memory>
#include <atomic>
#include "connection/connection.h"
#include "util/ikcp.h"

class Server;

inline int kcp_sync_output(const char* buf, int len, ikcpcb* kcp, void* user);

// TODO...有一个致命bug：ikcp不能支持两个strand
class KcpConnection: public Connection
{
        // TODO...如何处理心跳包
        static const std::string heartbeat_str;

    public:
        KcpConnection(boost::asio::io_service& io_server, std::weak_ptr<Server> server_wptr);
        ~KcpConnection();

        void bindConnect(const udp_addr&, const udp_addr&);

        // 主线程执行
        void startWork() override;
        // io线程执行
        void doStartWork() override;

        void syncSend(const char* buff, std::size_t len);
        int32_t asyncSend(std::shared_ptr<std::string> data_ptr);

        void asyncReceive() override;
        void disconnect() override;

    public:
        //kcp参数，必须在create_kcp之前设置
        inline void setInterval(uint32_t interval);
        inline uint32_t getInterval() const { return interval_; }

        inline void setResend(uint32_t resend);
        uint32_t getResend() const { return resend_; }

        inline void setNocwnd(bool nocwnd) { nocwnd_ = nocwnd ? 1 : 0; }
        inline bool getNocwnd() const { return nocwnd_; }

        inline void setSndwnd(uint32_t sndwnd);
        uint32_t getSndwnd() const { return sndwnd_; }

        inline void setRcvwnd(uint32_t rcvwnd);
        uint32_t getRcvwnd() const { return rcvwnd_; }

        inline void setSndBufferSize(uint32_t snd_size);
        uint32_t getSendBufferSise() const { return snd_buf_size_; }

        inline void setRcvBufferSize(uint32_t rcv_size);
        uint32_t getRcvBufferSize() const { return rcv_buf_size_; }

    protected:
        udp_space::socket& getSocket() { return socket_; }
        // 因为kcp等资源不支持同在读线程和写线程同时进行
        StrandPtr& getKcpStrand() { return kcp_strand_; }
        StrandPtr& getRecvStrand() = delete;
        StrandPtr& getSendStrand() = delete;
        std::array<char, 2048>& getRecvBuffer() { return recv_buffer_; }

        void setOption() override;
        void handleReceive(const boost_err&, std::size_t) override;
        void doAsyncSendData(std::shared_ptr<std::string>) override;
        void dispatchCloseSocket() override;
        void handleCloseSocket() override;

        inline bool createKcp();
        inline void releaseKcp();
        inline void startKcpUpdateTimer();
        inline void cancelKcpUpdateTimer()
        {
            is_kcp_updating = false;
            kcp_timer_.cancel();
        }
        void updateKcp();
        void onUpdateKcp(const boost_err& ec);

        inline void sendHeartbeat()
        {
            //直接通过socket发送，注意接收方的处理
            syncSend(heartbeat_str.data(), heartbeat_str.size());
        }

    private:
        udp_sock	socket_;
        udp_addr	bind_addr_;
        udp_addr 	rmt_addr_;
        std::array<char, 2048> recv_buffer_{{0}};

        uint64_t 	last_recv_millsec_ = 0;
        uint64_t 	last_send_millsec_ = 0;
        uint64_t 	timeout_millsec_ = 30000;
        uint64_t 	tick_millsec_ = 5000;
        StrandPtr   kcp_strand_;

        // 对kcp参数的设定
        ikcpcb* 			kcp_;
        uint32_t 			interval_ = 10;         // kcp_update最迟间隔
        uint32_t 			resend_ = 2;
        uint32_t 			nocwnd_ = 1;
        uint32_t 			stream_ = 1;
        uint32_t 			sndwnd_ = 64;
        uint32_t 			rcvwnd_ = 128;
        uint32_t 			snd_buf_size_ = 64;
        uint32_t 			rcv_buf_size_ = 64;

        boost::asio::deadline_timer		kcp_timer_;
        bool 					is_kcp_updating = false;
};

int kcp_sync_output(const char* buf, int len, ikcpcb*, void* user)
{
    static_cast<KcpConnection*>(user)->syncSend(buf, len);
    return 0;
}

#endif  // KCP_CONNECTION_H

/* vim: set ts=4 sw=4 sts=4 tw=100 */
