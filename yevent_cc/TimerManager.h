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
namespace yevent
{
class Timestamp;

class TimerEvent : public Event
{
public:
    TimerEvent(EventLoop *loop, int fd, Timestamp when, double interval)
        :Event(loop, fd, EV_READ),
        id_(-1),
        when_(when), 
        interval_(interval),
        repeat_(interval > 0.0)
    {
    }
    virtual ~TimerEvent(){}

    virtual void handleEvent();
    void setTimerId(int id) {id_ = id;}
    long getTimerId() {return id_;}
    Timestamp getTimerWhen() {return when_;}
    bool isRepeat() {return repeat_;}
    double getInterval() {return interval_;}
    void setValid(bool isValid) { isValid_ = isValid;}
    void isValid() { return isValid_;}
private:
    long id_;
    Timestamp when_
    double interval_;
    bool repeat_;
    bool isValid_;
};

class TimerManager 
{
    public:
        TimerManager(EventLoop *loop):timerFd_(CreateTimerfd()), pLoop_(loop), currentTimerId_(0), minHeap_(new MinHeap) {}
        void deleteTimer(TimerEvent *timer);
        //从最小堆中获取最小时间的timer
        TimerEvent* getNearestValidTimer();
        //生成timer, 并将该timer添加到最小堆中, 并将最小timer放到loop循环中。
        //return: timer id
        long addTimer(Timestamp when, double interval, TimerCallback cb, void *args);
        //重新调整MinHeap
        void reAddTimer(long timerId); 

        TimerEvent *getCurrentTimer(){return currentTimer_;}
        void setCurrentTimer(TimerEvent *timer) {currentTimer_ = timer;}

        TimerEvent *getTimer(long timerId);
    private:
        void resetTimerfd(TimerEvent *event);

        static const int RepeatTimer = 1;
        static const int OnceTimer = 2;
        int timerFd_;
        EventLoop *pLoop_;
        TimerEvent *currentTimer_;
        long currentTimerId_;
        boost::shared_ptr<MinHeap> minHeap_;
};

int CreateTimerfd();

}
#endif
