/*
 * =====================================================================================
 *
 *       Filename:  Multiplexer.cc
 *
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  04/25/2012 11:45:28 AM
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  Yin Zhongxing (olym), olym.yin@gmail.com
 *        Company:  
 *
 * =====================================================================================
 */
#ifndef __MULTIPLEXER_H
#define __MULTIPLEXER_H

#define YE_MAXEVENT 1024*10

#define Multiplexer_EPOLL 1
#define Multiplexer_SELECT 2

namespace yevent
{
class EventLoop;
class Multiplexer
{
    public:
        Multiplexer(){}
        virtual ~Multiplexer() {}

        virtual int initialize(EventLoop *loop) = 0;
        virtual int addEvent(int fd, int mask) = 0;
        virtual int updateEvent(int fd, int mask) = 0;
        virtual int deleteEvent(int fd, int mask) = 0;
        virtual int dispatch(int timeoutMs) = 0;
        virtual const char *getName() = 0;
};
Multiplexer *NewMultiplexerImp(int type);
}

#endif /* __MULTIPLEXER_H */
