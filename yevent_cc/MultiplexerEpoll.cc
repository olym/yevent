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
    /* If the fd was already monitored for some event, we need a MOD
     * operation. Otherwise we need an ADD operation. */
    int op = m_pLoop->events[fd].mask == YE_NONE ?
            EPOLL_CTL_ADD : EPOLL_CTL_MOD;

    ee.events = 0;
    mask |= eventLoop->events[fd].mask; /* Merge old events */
    if (mask & EV_READ) ee.events |= EPOLLIN;
    if (mask & EV_WRITE) ee.events |= EPOLLOUT;
    ee.data.u64 = 0; 
    ee.data.fd = fd;
    if (epoll_ctl(m_epollfd, op, fd, &ee) == -1) 
        return -1;
    return 0;
}

int MultiplexerEpoll::deleteEvent(int fd, int delmask)
{
    struct epoll_event ee;
    int mask = m_pLoop->events[fd].mask & (~delmask);

    ee.events = 0;
    if (mask & EV_READ) ee.events |= EPOLLIN;
    if (mask & EV_WRITE) ee.events |= EPOLLOUT;
    ee.data.u64 = 0;
    ee.data.fd = fd;
    if (mask != AE_NONE) {
        epoll_ctl(state->epfd,EPOLL_CTL_MOD,fd,&ee);
    } else {
        epoll_ctl(state->epfd,EPOLL_CTL_DEL,fd,&ee);
    }
    return 0;
}

int MultiplexerEpoll::dispatch(int timeoutMs)
{
    int retval, numevents = 0;

    retval = epoll_wait(m_epollfd, m_events, YE_MAXEVENT, timeoutMs);
    if (retval > 0) {
        int j;

        numevents = retval;
        for (j = 0; j < numevents; j++) {
            int mask = 0;
            struct epoll_event *e = m_events+j;

            if (e->events & EPOLLIN) mask |= EV_READ;
            if (e->events & EPOLLOUT) mask |= EV_WRITE;
            m_pLoop->fired_[j].fd = e->data.fd;
            m_pLoop->fired_[j].mask = mask;
        }
    }
    return numevents;
}

const char *MultiplexerEpoll::getName()
{
    return "Epoll";
}
