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
    struct Timer {
        long id;
        long when_sec;
        long when_ms;
        long interval;
        int timerType;
        TimerCallback timerCb;
        void *args;
    }
class TimerEvent : public Event
{
    public:
        TimerEvent(EventLoop *loop):Event(loop, CreateTimerfd(), EV_READ), currentTimerId_(-1), minHeap(new MinHeap) {}
        virtual void ~TimerEvent(){}
        virtual void handleEvent();
        long deleteTimer(long timerId);
        //从最小堆中获取最小时间
        Timer* getNearestTimer();
        //生成timer, 并将该timer添加到最小堆中和map<id, timer>中, 并更新fd_
        long addTimer(double expired, double interval, TimerCallback cb, void *args);
        //重新调整MinHeap
        void reAddTimer(long timerId); 

        Timer *getTimer(long timerId);
    private:
        void setTimerfd(long when_sec, long when_ms);

        static const int RepeatTimer = 1;
        static const int OnceTimer = 2;
        long currentTimerId_;
        boost::shared_ptr<MinHeap> minHeap_;
        std::map<long, Timer*> timerMap_;
};

int CreateTimerfd()
{
    int timerfd = timerfd_create(CLOCK_MONOTONIC, TFD_NONBLOCK | TFD_CLOEXEC);

    if (timerfd < 0) {
        fprintf(stderr, "failed in timerfd_create\n");
        return -1;
    }

    return timerfd;
}
}
#endif