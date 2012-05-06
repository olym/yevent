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
class TimerEvent : public Event
{
public:
    TimerEvent(EventLoop *loop, int fd, Timestamp when, double interval)
        :Event(loop, fd, EV_READ),
        when_(when), 
        interval_(interval),
        repeat_(interval > 0.0)
    {
    }
    virtual ~TimerEvent(){}

    virtual void handleEvent();
    virtual void setReadCallback();
    void setTimerId(int id);
private:
    long id_;
    Timestamp when_
    double interval_;
    bool repeat_;
};
class TimerManager : public Event
{
    public:
        TimerManager(EventLoop *loop):Event(loop, CreateTimerfd(), EV_READ), currentTimerId_(-1), minHeap(new MinHeap) {}
        long deleteTimer(long timerId);
        //从最小堆中获取最小时间
        TimerEvent* getNearestTimer();
        //生成timer, 并将该timer添加到最小堆中和map<id, timer>中, 并更新fd_
        long addTimer(double expired, double interval, TimerCallback cb, void *args);
        //重新调整MinHeap
        void reAddTimer(long timerId); 

        Timer *getTimer(long timerId);
    private:
        void (TimerEvent *event);

        static const int RepeatTimer = 1;
        static const int OnceTimer = 2;
        long currentTimerId_;
        boost::shared_ptr<MinHeap> minHeap_;
};

int CreateTimerfd();

}
#endif
