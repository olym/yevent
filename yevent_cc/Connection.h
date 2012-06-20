/*
 * =====================================================================================
 *
 *       Filename:  IOEvent.h
 *
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  2012年04月30日 16时04分04秒
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  YOUR NAME (), 
 *        Company:  
 *
 * =====================================================================================
 */
#ifndef __CONNECTION_H
#define __CONNECTION_H

#include "EventLoop.h"
#include "Event.h"
#include "InetSockAddr.h"
#include "Buffer.h"
#include "TcpServer.h"
#include <string>

namespace yevent {
    class Connection : public Event
    {
        public:
           Connection(EventLoop *loop, int connfd, TcpServer *server, InetSockAddr peerAddr);
           virtual ~Connection();
           //virtual void handleEvent();
           virtual void handleRead();
           virtual void handleWrite();
           void handleClose();
           void enableWriting();
           void disableWriting(); 
           bool isWriting() { return isWriting_;}
           int send(const void *data, int len);
           int send(std::string str);
           static void registerConnection(void *conn);
        private: 
           void registerConnection();
           TcpServer *server_;
           InetSockAddr peerAddr_;
           Buffer inputBuf_;
           Buffer outputBuf_;
           bool isWriting_;
    };
}

#endif
