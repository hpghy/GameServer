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
#include "connection.h"
#include "../util/ikcp.h"

class Server;

inline int kcp_sync_output(const char* buf, int len, ikcpcb* kcp, void* user);

class KcpConnection: public Connection
{
    public:
        KcpConnection(boost::asio::io_service& io_server, std::weak_ptr<Server> server_wptr);
        ~KcpConnection();

        void bindConnect(const udp_addr&, const udp_addr&);
        void doStartWork() override;
        void syncSend(const char* buff, std::size_t len);

        void asyncReceive() override;

        //kcp参数，必须在create_kcp之前设置
        void setInterval(unsigned int interval)
        {
            if (interval > 0)
                interval_ = interval;
        }
        unsigned int getInterval() { return interval_; }

        void setResend(unsigned int resend)
        {
            if (resend > 0)
                resend_ = resend;
        }
        unsigned int getResend() { return resend_; }

        void setNocwnd(bool nocwnd)
        {
            if (nocwnd)
                nocwnd_ = 1;
            else
                nocwnd_ = 0;
        }
        bool getNocwnd() { return nocwnd_; }

        void setSndwnd(unsigned int sndwnd)
        {
            if (sndwnd > 0)
                sndwnd_ = sndwnd;
        }
        unsigned int getSndwnd() { return sndwnd_; }

        void setRcvwnd(unsigned int rcvwnd)
        {
            if (rcvwnd > 0)
                rcvwnd_ = rcvwnd;
        }
        unsigned int getRcvwnd() { return rcvwnd_; }

        void setSndBufferSize(unsigned int snd_size)
        {
            if (snd_size > 0)
            {
                snd_buf_s_ = snd_size;
            }
        }
        unsigned int getSendBufferSise() { return snd_buf_s_; }

        void setRcvBufferSize(unsigned int rcv_size)
        {
            if (rcv_size > 0)
            {
                rcv_buf_s_ = rcv_size;
            }
        }
        unsigned int getRcvBufferSize() { return rcv_buf_s_; }


    protected:
        void setOption() override;
        void handleReceive(const boost_err&, std::size_t) override;
        void doAsyncSendData(std::shared_ptr<std::string>) override;

        inline bool createKcp()
        {
            if (kcp_)
            {
                return false;
            }
            kcp_ = ikcp_create(0, (void*)this);
            ikcp_nodelay(kcp_, 1, interval_, resend_, nocwnd_);
            ikcp_wndsize(kcp_, sndwnd_, rcvwnd_);
            kcp_->output = kcp_sync_output;
            kcp_->stream = stream_;
            return true;
        }

        inline void releaseKcp()
        {
            if (!kcp_)
                return;
            ikcp_release(kcp_);
            kcp_ = nullptr;
        }

        inline void startKcpUpdate()
        {
            if (kcp_updating == true)
                return;
            kcp_updating = true;
            updateKcp();
        }

        inline void cancelKcpUpdate()
        {
            kcp_updating = false;
            kcp_timer_.cancel();
        }

        inline void sendHeartbeat()
        {
            //直接通过socket发送，注意接收方的处理
            syncSend(heartbeat_str.data(), heartbeat_str.size());
        }

        void updateKcp();
        void onUpdateKcp(const boost_err& ec);

        void handleCloseSocket() override;

    protected:
        udp_sock	socket_;
        udp_addr	bind_addr_;
        udp_addr 	rmt_addr_;
        std::array<char, 2048> recv_buffer_{{0}};

        uint64_t 	last_recv_;
        uint64_t 	last_send_;
        uint64_t 	timeout_mill_ = 30000;
        uint64_t 	tick_mill_ = 5000;

        static const std::string heartbeat_str;

        //TODO...对kcp参数的设定
        ikcpcb* 			kcp_;
        unsigned int 			interval_ = 10;
        unsigned int 			resend_ = 2;
        unsigned int 			nocwnd_ = 1;
        unsigned int 			stream_ = 1;
        unsigned int 			sndwnd_ = 64;
        unsigned int 			rcvwnd_ = 128;
        unsigned int 			snd_buf_s_ = 1024;
        unsigned int 			rcv_buf_s_ = 1024;
        boost::asio::deadline_timer		kcp_timer_;

        bool 					kcp_updating = false;
};

int kcp_sync_output(const char* buf, int len, ikcpcb*, void* user)
{
    static_cast<KcpConnection*>(user)->syncSend(buf, len);
    return 0;
}

#endif  // KCP_CONNECTION_H

/* vim: set ts=4 sw=4 sts=4 tw=100 */
