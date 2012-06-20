/*
 * =====================================================================================
 *
 *       Filename:  TcpServer_test.cc
 *
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  06/17/2012 10:15:02 PM
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  Yin Zhongxing (olym), zhongxing.yin@cs2c.com.cn
 *        Company:  www.CS2C.com.cn
 *
 * =====================================================================================
 */
#include "../EventLoop.h"
#include "../TcpServer.h"
#include "../Buffer.h"
#include "../Connection.h"
#include <iostream>
#include <string>

using namespace yevent;
using namespace std;

class TcpServerDemo : public TcpServer
{
    public:
        TcpServerDemo(EventLoop *loop, int port, string name) :
            TcpServer(loop, port, name)
        {
        }
        ~TcpServerDemo() {}
        virtual void dataReceived(Connection* conn, Buffer *buffer) {
            string str = buffer->getAllAsString();
            cout << "dataReceived:" << str;
            conn->send(str.c_str(), str.size());
        }
        virtual void connectionMake(Connection* conn) {
            cout << "connectionMake" << endl; 
        }
        virtual void connectionLost(Connection* conn) {
            cout << "connectionLost" << endl; 
        }
        virtual void dataWriteDone(Connection *conn) {
            cout << "dataWriteDone" << endl;
        }
};

int main(int argc, char *argv[])
{
    EventLoop loop;
    TcpServerDemo demoServer(&loop, 12345, "demo");
    demoServer.setThreadNums(2);
    demoServer.start();
    loop.dispatch(); 
    return 0;
}
