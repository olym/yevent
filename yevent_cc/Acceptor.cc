/*
 * =====================================================================================
 *
 *       Filename:  Acceptor.cc
 *
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  2012年05月31日 16时39分36秒
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  Yin Zhongxing (olym), zhongxing.yin@cs2c.com.cn
 *        Company:  www.CS2C.com.cn
 *
 * =====================================================================================
 */
#include "EventLoop.h"
#include "Acceptor.h"
#include "TcpServer.h"
#include "SocketUtil.h"
#include "Utility.h"
#include <errno.h>
#include <string>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>


using namespace yevent;
using namespace yevent::socket;
using namespace yevent::util;

Acceptor::Acceptor(EventLoop *loop, TcpServer *server, int port) :
    loop_(loop),
    server_(server),
    listenfd_(socket::createNoblockingSocket()),
    idleFd_(-1),
    acceptEvent_(loop_, listenfd_, EV_READ),
    port_(port),
    isListenning_(false)
{
    acceptEvent_.setReadCallback(Acceptor::handleRead, this);
}

Acceptor::~Acceptor()
{
    isListenning_ = false;
    loop_->deleteEvent(&acceptEvent_);
    socket::close(listenfd_);
}

void Acceptor::listen()
{
    if (isListenning_ == false) {
        yeventLog(YEVENT_DEBUG, "Acceptor::listen");
        socket::bind(listenfd_, port_);
        socket::listen(listenfd_);
        loop_->registerEvent(&acceptEvent_);
        isListenning_ = true;
    }
}

void Acceptor::handleRead(void *obj)
{
    Acceptor *acceptor = static_cast<Acceptor *>(obj);
    acceptor->handleRead();
}

void Acceptor::handleRead()
{
    InetSockAddr peerAddr(0);
    //FIXME 有可能同时来了多个连接 
    int connfd = socket::accept(listenfd_, &peerAddr);
    if (connfd >= 0) {
        server_->newConnection(connfd, peerAddr);
    }
    else {
        //处理文件描述符不够用的问题
        if (errno == EMFILE) {
            ::close(idleFd_);
            idleFd_ = ::accept(listenfd_, NULL, NULL);
            ::close(idleFd_);
            idleFd_ = ::open("/dev/null", O_RDONLY);
        }
    }
}

