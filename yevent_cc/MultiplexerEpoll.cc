/*
 * =====================================================================================
 *
 *       Filename:  MultiplexerEpoll.cc
 *
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  04/25/2012 02:43:05 PM
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  Yin Zhongxing (olym), olym.yin@gmail.com
 *        Company:  
 *
 * =====================================================================================
 */

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <assert.h>
#include "MultiplexerEpoll.h"
#include "Event.h"
#include "EventLoop.h"

using namespace yevent;

MultiplexerEpoll::MultiplexerEpoll()
    :pLoop_(NULL),
    epollfd_(-1),
    nEvents_(YE_MAXEVENT)
{

}
MultiplexerEpoll::~MultiplexerEpoll()
{
    ::close(epollfd_);
}
int MultiplexerEpoll::initialize(EventLoop *loop)
{
    assert(loop);

    pLoop_ = loop;
    epollfd_ = ::epoll_create(YE_MAXEVENT);
    if (epollfd_ == -1) {
        //YE_LOG();
        return -1;
    }
    return 0;
}

int MultiplexerEpoll::addEvent(int fd, int mask)
{
    struct epoll_event ee;
    ee.events = 0;
    if (mask & EV_READ) ee.events |= EPOLLIN;
    if (mask & EV_WRITE) ee.events |= EPOLLOUT;
    ee.data.u64 = 0;
    ee.data.fd = fd;
    if (epoll_ctl(epollfd_, EPOLL_CTL_ADD, fd, &ee) == -1 && errno != EEXIST) {
        fprintf(stderr, "epoll_ctl(%d,%d) failed: %d\n", EPOLL_CTL_ADD,fd,errno);
        return -1;
    }
    return 0;
}

int MultiplexerEpoll::deleteEvent(int fd, int delmask)
{
    struct epoll_event ee;

    ee.events = 0;
    ee.data.fd = fd;
    ee.data.u64 = 0;
    if (epoll_ctl(epollfd_, EPOLL_CTL_DEL, fd, &ee) == -1
            && errno != ENOENT && errno != EBADF) {
        fprintf(stderr, "epoll_ctl(%d,%d) failed: %d\n", EPOLL_CTL_DEL,fd,errno);
        return -1;
    }
    return 0;
}
int MultiplexerEpoll::updateEvent(int fd, int mask)
{
    struct epoll_event ee;

    //ee.events = EPOLLONESHOT;
    if (mask & EV_READ)
        ee.events |= EPOLLIN;
    if (mask & EV_WRITE)
        ee.events |= EPOLLOUT;
    ee.data.u64 = 0;
    ee.data.fd = fd;
    if (epoll_ctl(epollfd_, EPOLL_CTL_MOD, fd, &ee) == -1 && errno != EEXIST) {
        fprintf(stderr, "epoll_ctl(%d, %d) failed: %d\n", EPOLL_CTL_ADD, fd, errno);
        return -1;
    }
    return 0;
}

int MultiplexerEpoll::dispatch(int timeoutMs)
{
    int retval, numevents = 0;

    retval = epoll_wait(epollfd_, events_, YE_MAXEVENT, timeoutMs==0 ? -1: timeoutMs);
    if (retval > 0) {
        for (int i = 0; i < retval; i++) {
            int mask = 0;

            if (events_[i].events & EPOLLIN) mask |= EV_READ;
            if (events_[i].events & EPOLLOUT) mask |= EV_WRITE;

            if (mask == 0)
                continue;

            numevents++;
            Event *ev = pLoop_->getRegisteredEvents(events_[i].data.fd);
            pLoop_->activeEvent(ev);
            //ev->setReturnFlags(events_[i].events);
        }
    }
    return numevents;
}

const char* MultiplexerEpoll::getName()
{
    return "Epoll";
}
