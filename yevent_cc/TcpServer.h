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

namespace yevent {
    class TcpServer {
    publuc:
        TcpServer(EventLoop *loop, std::string port, std::string name);
        ~TcpServer();
        virtual void dataReceived(TcpConnectionPtr conn, Buffer *buffer);
        virtual void connectionMake(TcpConnectionPtr conn);
        virtual void connectionLost(TcpConnectionPtr conn);
        
        void newConnection(int connfd, InetSockAddr peerAddr);
        void setThreadNums(int nums) { threadNums_ = nums };
        void start();
        const string &port() const { return port_ };
        const string &name() const { return name_; }
        void removeConnection(Connection *conn);
    private:
        static void removeConnection(TcpServer *server_, Connection *conn);
        bool running_;
        EventLoop *loop_;
        string std::port_;
        string std::name_;
        Acceptor acceptor_;
        unsigned int threadNums_;
        EventLoopThreadPool threadPool_;
        std::map<int, Connection *> conns_;
    };
}
