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
        Event(EventLoop *loop, int fd, int event) : pLoop_(loop), fd_(fd), event_(event);
        virtual ~Event();
        virtual void handleEvent() = 0;
        void setReadCallback(EventCallback cb, void *args) { readCallback_ = cb; evReadArgs_ = args;}
        void setWriteCallback(EventCallback cb, void *args) { writeCallback_ = cb; evWriteArgs_ = args; }
        void update() { pLoop_->updateEvent(this);}
        void delete() { pLoop_->deleteEvent(this);}
        int getEvent() { return event_;}
        int getFd() { return fd_; }
        EventLoop *getEventLoop() {return pLoop_;}
        void *getArgs() { return evArgs_; }
    private:
        int fd_;
        int event_; //EV_READ; EV_WRITE;
        EventLoop *pLoop_;
        EventCallBack readCallback_;
        EventCallBack writeCallback_;
        void *evReadArgs_;
        void *evWriteArgs_;
}

#endif/* __EVENT_H */
