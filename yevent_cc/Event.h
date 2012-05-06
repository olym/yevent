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
struct FireEvent 
{
    int fd;
    int mask;
};

class Event
{
    public:
        Event(EventLoop *loop, int fd, int event);
        virtual ~Event();
        virtual int getEvent();
        virtual void setReadCallback();
        virtual void setWriteCallback();
        virtual void handleEvent();
    //private:
        int fd_;
        int event_; //EV_READ; EV_WRITE;
        EventLoop *pLoop_;
        EventCallBack readCallback_;
        EventCallBack writeCallback_;
        void *evArgs_;
}

#endif/* __EVENT_H */
