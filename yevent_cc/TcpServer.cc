/*
 * =====================================================================================
 *
 *       Filename:  TcpServer.cc
 *
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  2012年05月31日 16时01分12秒
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  Yin Zhongxing (olym), zhongxing.yin@cs2c.com.cn
 *        Company:  www.CS2C.com.cn
 *
 * =====================================================================================
 */
#include "TcpServer.h"
#include "Buffer.h"
#include "MutexLock.h"
#include "Connection.h"
#include "InetSockAddr.h"
#include "SocketUtil.h"
#include "Utility.h"

using namespace yevent;
using std::string;

TcpServer::TcpServer(EventLoop *loop, int port, string name) :
    loop_(loop),
    port_(port),
    name_(name),
    acceptor_(loop_, this, port_),
    threadNums_(0),
    threadPool_(loop_),
    isRunning_(false),
    mutex_()
{

}

TcpServer::~TcpServer()
{
 isRunning_ = false;
}

void TcpServer::start()
{
    if (isRunning_ == false) {
        util::yeventLog(YEVENT_DEBUG, "TcpServer start");
        acceptor_.listen();
        threadPool_.start(threadNums_); 
        isRunning_ = true;
    }
}

//run in main loop
void TcpServer::newConnection(int connfd, InetSockAddr peerAddr)
{
    util::yeventLog(YEVENT_DEBUG, "newConnection");
    //loop_->assertInLoopThread();
    assert(loop_->isInLoopThread());

    EventLoop *ioLoop;
    ioLoop = threadPool_.getNextLoop();
    Connection * conn = new Connection(ioLoop, connfd, this, peerAddr);
    {
    MutexLockGuard lock(mutex_);
    conns_[connfd] = conn;
    }
    //util::yeventLog(YEVENT_DEBUG, "%s:threadId_ = %d tid = %d", __func__, threadId_, util::gettid());
    ioLoop->runInLoop(&Connection::registerConnection, static_cast<void *>(conn));
}

//run in ioLoop
void TcpServer::removeConnection(Connection *conn)
{
    assert(conn->getEventLoop()->isInLoopThread());

    {
    MutexLockGuard lock(mutex_);
    conns_.erase(conn->getFd());
    }

    socket::close(conn->getFd());

    connectionLost(conn);
    delete conn;
}

