/*
 * =====================================================================================
 *
 *       Filename:  TcpClient.h
 *
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  04/26/2012 12:43:17 AM
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  YOUR NAME (), 
 *        Company:  
 *
 * =====================================================================================
 */

#ifndef __TPC_CLIENT_H
#define __TPC_CLIENT_H
namespace yevent {
    class TcpClient {
    public:
        TcpClient(EventLoop *loop, std::string name);
        virtual TcpClient();
        virtual void dataReceived(Connection* conn, Buffer *buffer) {}
        virtual void dataWriteDone(Connection *conn) {}
        virtual void connectionMake(Connection* conn) {}
        virtual void connectionLost(Connection* conn) {}
        void newConnection();
        void connect(std::string host, int port);
        void disconnect();
    private:
        EventLoop *loop_;
        std::string name_;
        bool isConnected_;
        Connector connector_;
        Connection *conn_;
    };
}

#endif /* TCP_CLIENT_H */
