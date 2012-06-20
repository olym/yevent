/*
 * =====================================================================================
 *
 *       Filename:  Connection.cc
 *
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  2012年06月12日 21时59分20秒
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  Yin Zhongxing (olym), zhongxing.yin@cs2c.com.cn
 *        Company:  www.CS2C.com.cn
 *
 * =====================================================================================
 */
#include "Connection.h"
#include <errno.h>
#include <string.h>

using namespace yevent;
Connection::Connection(EventLoop *loop, int connfd, TcpServer *server, InetSockAddr peerAddr_) :
    Event(loop, connfd, EV_READ), 
    server_(server), 
    peerAddr_(peerAddr_),
    isWriting_(false)
{
    
}

Connection::~Connection()
{

}

//static
void Connection::registerConnection(void *obj)
{
    util::yeventLog(YEVENT_DEBUG, "Connection::registerConnection");
    Connection *conn = static_cast<Connection *>(obj);
    conn->registerConnection();
}

void Connection::registerConnection()
{
    updateEvent();
    server_->connectionMake(this);
}

int Connection::send(const void *data, int len)
{
    EventLoop *loop = getEventLoop();
    assert(loop->isInLoopThread());
    int nwrote = 0;
    if (!isWriting() && outputBuf_.readableBytes() == 0) {
        nwrote = ::write(getFd(), data, len);

        if (nwrote > 0) {
            if (nwrote == len) 
                server_->dataWriteDone(this);
            else 
                util::yeventLog(YEVENT_DEBUG, "data does not write done one time, and the rest data will be put in output buf");
        } else {
            nwrote = 0;
            if (errno != EWOULDBLOCK) {
                util::yeventLog(YEVENT_WARNING, "write error %s", strerror(errno));
            }
        }
    }

    assert(nwrote >= 0);
    if (nwrote < len) {
        outputBuf_.append(static_cast<const char *>(data)+nwrote, len-nwrote);
        if (!isWriting())
            enableWriting();
    }
}

int Connection::send(std::string str)
{
     send(str.c_str(), str.size());
}

void Connection::enableWriting()
{
    if (!(getEvent() & EV_WRITE)) {
        setEvent(getEvent() | EV_WRITE);
        updateEvent();
        isWriting_ = true;
    }
}

void Connection::disableWriting()
{
    if (getEvent() & EV_WRITE) {
        setEvent(getEvent() & ~EV_WRITE);
        updateEvent();
        isWriting_ = false;
    }
}

void Connection::handleRead()
{
    assert(getEventLoop()->isInLoopThread()); 
    util::yeventLog(YEVENT_DEBUG, "Connection::handleRead");
    int error;

    int nread = inputBuf_.read(getFd(), &error);
    if (nread > 0) {
        server_->dataReceived(this, &inputBuf_);
    } else if (nread == 0){
        handleClose();
    } else {
        if (error != EAGAIN) {
            handleClose();
            return;
        }
    }
}

void Connection::handleClose()
{
    assert(getEventLoop()->isInLoopThread()); 
    deleteEvent();
    server_->removeConnection(this);
}
void Connection::handleWrite()
{
    assert(getEventLoop()->isInLoopThread()); 
    util::yeventLog(YEVENT_DEBUG, "%s", __func__);

    if (isWriting()) {
        ssize_t n = ::write(getFd(), outputBuf_.beginRead(), outputBuf_.readableBytes());
        
        if (n > 0) {
            outputBuf_.retrieve(n);
            if (outputBuf_.readableBytes() == 0) {
                server_->dataWriteDone(this);
                disableWriting();
            }
        } else {
            util::yeventLog(YEVENT_DEBUG, "%s:write error", __func__);
        }
    } else {
        util::yeventLog(YEVENT_DEBUG, "write error, maybe connection is lost");
    }
}
