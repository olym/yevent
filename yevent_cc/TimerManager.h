/*
 * =====================================================================================
 *
 *       Filename:  TimerManager.h
 *
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  04/26/2012 12:39:22 AM
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  YOUR NAME (), 
 *        Company:  
 *
 * =====================================================================================
 */
#ifndef __TIMER_MANAGER_H
#define __TIMER_MANAGER_H
#include <boost/scoped_ptr.hpp>
#include "Timestamp.h"
#include "Event.h"
#include "MinHeap.h"
namespace yevent
{
int CreateTimerfd();

typedef void (*TimerCallback)(void *args);
class EventLoop;

class TimerEvent : public Event
{
public:
    TimerEvent(EventLoop *loop, int fd, Timestamp when, double interval)
        :Event(loop, fd, EV_READ),
        id_(-1),
        when_(when), 
        interval_(interval),
        repeat_(interval > 0.0),
        isValid_(true)
    {
    }
    virtual ~TimerEvent(){}

    virtual void handleEvent();
    void setTimerId(int id) {id_ = id;}
    long getTimerId() {return id_;}
    Timestamp getWhen() {return when_;}
    void setWhen(Timestamp when) {when_ = when;}
    bool isRepeat() {return repeat_;}
    double getInterval() {return interval_;}
    void setValid(bool isValid) { isValid_ = isValid;}
    bool isValid() { return isValid_;}
private:
    long id_;
    Timestamp when_;
    double interval_;
    bool repeat_;
    bool isValid_;
};

class TimerManager 
{
    public:
        TimerManager(EventLoop *loop):timerFd_(CreateTimerfd()), pLoop_(loop), currentTimerId_(0), minHeap_(new MinHeap(NULL, NULL)) {
            minHeap_->init();
        }
        void deleteTimer(TimerEvent *timer);
        TimerEvent* getNearestValidTimer();
        long addTimer(Timestamp when, double interval, TimerCallback cb, void *args);
        void handleTimerEvents();

    private:
        void resetTimer(TimerEvent *event);
        void updateTimerEvent();

        static const int RepeatTimer = 1;
        static const int OnceTimer = 2;
        int timerFd_;
        EventLoop *pLoop_;
        TimerEvent *currentTimer_;
        long currentTimerId_;
        boost::scoped_ptr<MinHeap> minHeap_;
};


}
#endif
