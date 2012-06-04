/*
 * =====================================================================================
 *
 *       Filename:  Event.h
 *
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  04/25/2012 09:58:29 PM
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  YOUR NAME (), 
 *        Company:  
 *
 * =====================================================================================
 */
#ifndef __EVENT_H
#define __EVENT_H

#include "Event_Struct.h"
namespace yevent{


typedef void (*EventCallback)(void *args);
struct FireEvent 
{
    int fd;
    int mask;
};
class EventLoop;
class Event
{
    public:
        Event(EventLoop *loop, int fd, int event): pLoop_(loop), fd_(fd), event_(event) {}
        virtual ~Event(){}
        virtual void handleEvent() {
            if (event_ & EV_READ) handleRead();
            if (event_ & EV_WRITE) handleWrite();
        }
        void setReadCallback(EventCallback cb, void *args) { readCallback_ = cb; evReadArgs_ = args;}
        void setWriteCallback(EventCallback cb, void *args) { writeCallback_ = cb; evWriteArgs_ = args; }
        //void updateEvent();  
        //void deleteEvent();        
        int getEvent() { return event_;}
        int getFd() { return fd_; }
        virtual void handleRead() { if (readCallback_) readCallback_(evReadArgs_);}
        virtual void handleWrite() { if (writeCallback_) writeCallback_(evWriteArgs_);}
        EventLoop *getEventLoop() {return pLoop_;}
    private:
        int fd_;
        int event_; //EV_READ; EV_WRITE;
        EventLoop *pLoop_;
        EventCallback readCallback_;
        EventCallback writeCallback_;
        void *evReadArgs_;
        void *evWriteArgs_;
};
}
#endif/* __EVENT_H */
