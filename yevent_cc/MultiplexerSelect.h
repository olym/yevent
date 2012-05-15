/*
 * =====================================================================================
 *
 *       Filename:  MultiplexerSelect.cc
 *
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  04/25/2012 03:41:33 PM
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  Yin Zhongxing (olym), olym.yin@gmail.com
 *        Company:  
 *
 * =====================================================================================
 */
#ifndef __MULTIPLEXER_SELECT_H
#define __MULTIPLEXER_SELECT_H

#include "Multiplexer.h"

namespace yevent
{
class EventLoop;
class MultiplexerSelect : public Mutiplexer
{
    public:
        MultiplexerSelect();
        virtual ~MultiplexerEpoll();
        virtual int initialize(EventLoop *loop);
        virtual int addEvent(int fd, int mask);
        virtual int deleteEvent(int fd, int mask);
        virtual int dispatch(int timeoutMs);
        virtual const char *getName() = 0;
    private:
        EventLoop *pLoop;
        int m_epollfd;
        struct epoll_event m_events[YE_MAXEVENT];
        int m_nEvents;
};
}
#endif /* __MULTIPLEXER_SELECT_H */
