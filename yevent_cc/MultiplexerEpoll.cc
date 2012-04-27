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

#include "MultiplexerEpoll.h"

using namespace yevent;

MultiplexerEpoll::MultiplexerEpoll()
    :m_pLoop(NULL),
    m_epollfd(-1),
    m_nevent(YE_MAXEVENT)
{

}
MultiplexerEpoll::~MultiplexerEpoll()
{
    ::close(m_epollfd);
}
MultiplexerEpoll::initialize(EventLoop *loop)
{
    assert(loop);

    m_pLoop = loop;
    m_epollfd = ::epoll_create(1024);
    if (m_epollfd == -1) {
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
    if (epoll_ctl(m_epollfd, EPOLL_CTL_ADD, fd, &ee) == -1 && errno != EEXIST) {
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
    /* Note, Kernel < 2.6.9 requires a non null event pointer even for
     * EPOLL_CTL_DEL. */
    if (epoll_ctl(m_epollfd, EPOLL_CTL_DEL, fd, &ee) == -1
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
    if (epoll_ctl(m_epollfd, EPOLL_CTL_MOD, fd, &ee) == -1 && errno != EEXIST) {
        fprintf(stderr, "epoll_ctl(%d, %d) failed: %d\n", EPOLL_CTL_ADD, fd, errno);
        return -1;
    }
    return 0;
}

int MultiplexerEpoll::dispatch(int timeoutMs)
{
    int retval, numevents = 0;

    retval = epoll_wait(m_epollfd, m_events, YE_MAXEVENT, timeoutMs);
    if (retval > 0) {
        fillActiveEvents(numevents);
    }
    return numevents;
}

void MultiplexerEpoll::fillActiveEvents(int numevents)
{
    for (int i = 0; i < numevents; i++) {
        int mask = 0;

        if (m_events[i] & EPOLLIN) mask |= EV_READ;
        if (m_events[i] & EPOLLOUT) mask |= EV_WRITE;

        if (mask == 0)
            continue;

        Event *ev = m_pLoop->registeredEvents_[fd];
        ev.setReturnFlags(m_events[i].events);
        activeEvents_.push_back(ev);
    }
    
}

const char* MultiplexerEpoll::getName()
{
    return "Epoll";
}
