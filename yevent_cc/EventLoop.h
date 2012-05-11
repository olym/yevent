/*
 * =====================================================================================
 *
 *       Filename:  EventLoop.cc
 *
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  04/25/2012 09:55:58 AM
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  Yin Zhongxing (olym), olym.yin@gmail.com
 *        Company:  
 *
 * =====================================================================================
 */

#ifndef __EVENTLOOP_H
#define __EVENTLOOP_H

namespace yevent
{
class EventLoop
{
public:
    EventLoop();
    long runAt(Timestamp &ts, TimerCallback cb, void *privdata);
    long runEvery(double interval, TimerCallback cb, void *privdata);
    long runAfter(double delay, TimerCallback cb, void *privdata);
    void deleteTimer(long timerId);
    void registerEvent(Event *ev)
    void unregisterEvent(Event *ev); 
    Event* registerSignalEvent(int signo, SignalCallback cb, void *privdata);
    dispatch();

private:
    void updateEvent(Event *event); // register and update
    void deleteEvent(Event *event); // remove event from registeredEvents_, and call mutiplexer_->deleteEvent;
    void handlerNotify();

    bool stop_;
    int notifyfd_; //notify dispatch wake up
    Event notifyEvent_;
    boost::scoped_ptr<Mutiplexer> mutiplexer_;
    boost::scoped_ptr<TimerManager> timerManager_;
    std::map<int, Event*> registeredEvents_;
    std::vector<Event *> activeEvents_;
    unsigned long threadId_;
    MutexLock mutex_;
};
}



#endif /* __EVENTLOOP_H */
