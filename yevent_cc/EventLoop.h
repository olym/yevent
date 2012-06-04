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
#include <boost/scoped_ptr.hpp>
#include <map>
#include <vector>
#include <assert.h>
#include "Event.h"
#include "Multiplexer.h"
#include "TimerManager.h"
#include "MutexLock.h"
#include "Task.h"

namespace yevent
{
typedef void (*TimerCallback)(void *args);
typedef void (*SignalCallback)(void *args);
class Timestamp;

class EventLoop
{
public:
    EventLoop();
    ~EventLoop();
    void init();
    long registerTimerEvent(double timeout, double interval, TimerCallback cb, void *privdata);
    void deleteTimer(long timerId);
    Event* registerSignalEvent(int signo, SignalCallback cb, void *privdata);
    void registerEvent(Event *ev);
    void deleteEvent(Event *ev); // remove event from registeredEvents_, and call mutiplexer_->deleteEvent;
    void dispatch();
    void updateEvent(Event *event); // register and update
    Event *getRegisteredEvents(int fd) {return registeredEvents_[fd];}
    void activeEvent(Event *event) { assert(event != NULL); activeEvents_.push_back(event);}

    //used stop the loop that in another thread
    void breakLoop();
    bool isInLoopThread();
    //put task that will be excuted into the deferredQueue and notify the loop to excute tall asks in the deferredQueue 
    void runInLoop(Task task);
    void runDeferredTasks();
    void assertInLoopThread();

    int notifyfd_; 
    int isNotifyPending_;
private:
    //used to wake up thread loop;
    void wakeup();

    bool stop_;
    boost::scoped_ptr<Event> notifyEvent_;
    boost::scoped_ptr<Multiplexer> multiplexer_;
    boost::scoped_ptr<TimerManager> timerManager_;
    std::map<int, Event*> registeredEvents_;
    std::vector<Event *> activeEvents_;
    std::vector<Task> deferredQueue_;
    unsigned long threadId_;
    MutexLock mutex_;
};
int createEventfd();
void handleNotify(void *);
}



#endif /* __EVENTLOOP_H */
