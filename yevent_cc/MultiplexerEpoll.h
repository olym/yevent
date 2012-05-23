/*
 * =====================================================================================
 *
 *       Filename:  Multiplexer_Epoll.h
 *
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  04/25/2012 01:49:32 PM
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  YOUR NAME (), 
 *        Company:  
 *
 * =====================================================================================
 */
#ifndef __MULTIPLEXER_EPOLL_H
#define __MULTIPLEXER_EPOLL_H

#include <sys/epoll.h>
#include "Multiplexer.h"

namespace yevent
{
class EventLoop;
class MultiplexerEpoll : public Multiplexer
{
    public:
        MultiplexerEpoll(); 
        virtual ~MultiplexerEpoll();
        virtual int initialize(EventLoop *loop);
        virtual int addEvent(int fd, int mask);
        virtual int updateEvent(int fd, int mask);
        virtual int deleteEvent(int fd, int mask);
        virtual int dispatch(int timeoutMs);
        virtual const char *getName();
    private:
        EventLoop *pLoop_;
        int epollfd_;
        struct epoll_event events_[YE_MAXEVENT];
        int nEvents_;
};
}
#endif /* __MULTIPLEXER_EPOLL_H */
