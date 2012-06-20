/*
 * =====================================================================================
 *
 *       Filename:  TcpServer.h
 *
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  04/26/2012 12:43:27 AM
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  YOUR NAME (), 
 *        Company:  
 *
 * =====================================================================================
 */
#ifndef __TCP_SERVER_H
#define __TCP_SERVER_H

#include "EventLoopThreadPool.h"
#include "Acceptor.h"
#include <string>
#include <map>

namespace yevent {
    class InetSockAddr;
    class Buffer;
    class Connection;
    //class MutexLock;
    class EventLoop;
    class TcpServer {
    public:
        TcpServer(EventLoop *loop, int port, std::string name);
        ~TcpServer();
        virtual void dataReceived(Connection* conn, Buffer *buffer) {}
        virtual void dataWriteDone(Connection *conn) {}
        virtual void connectionMake(Connection* conn) {}
        virtual void connectionLost(Connection* conn) {}
        
        void newConnection(int connfd, InetSockAddr peerAddr);
        void setThreadNums(int nums) { threadNums_ = nums; };
        void start();
        const int &port() const { return port_; };
        const std::string &name() const { return name_; }
        void removeConnection(Connection *conn);
    private:
        EventLoop *loop_;
        int port_;
        std::string name_;
        Acceptor acceptor_;
        unsigned int threadNums_;
        EventLoopThreadPool threadPool_;
        std::map<int, Connection *> conns_;
        bool isRunning_;
        MutexLock mutex_;  // used to guard conns_;
    };
}

#endif
