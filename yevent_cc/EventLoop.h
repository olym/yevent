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

namespace yevent
{
typedef void (*TimerCallback)(void *args);
typedef void (*SignalCallback)(void *args);
class Timestamp;

class EventLoop
{
public:
    EventLoop();
    void init();
    long runAt(Timestamp &ts, TimerCallback cb, void *privdata);
    long runEvery(double interval, TimerCallback cb, void *privdata);
    long runAfter(double delay, TimerCallback cb, void *privdata);
    void deleteTimer(long timerId);
    void registerEvent(Event *ev);
    void unregisterEvent(Event *ev); 
    Event* registerSignalEvent(int signo, SignalCallback cb, void *privdata);
    void dispatch();
    void updateEvent(Event *event); // register and update
    void deleteEvent(Event *event); // remove event from registeredEvents_, and call mutiplexer_->deleteEvent;
    Event *getRegisteredEvents(int fd) {return registeredEvents_[fd];}
    void activeEvent(Event *event) { assert(event != NULL); activeEvents_.push_back(event);}

    //used to wake up thread loop;
    void wakeup();

    int notifyfd_; 
    int isNotifyPending_;
private:

    bool stop_;
    boost::scoped_ptr<Event> notifyEvent_;
    boost::scoped_ptr<Multiplexer> multiplexer_;
    boost::scoped_ptr<TimerManager> timerManager_;
    std::map<int, Event*> registeredEvents_;
    std::vector<Event *> activeEvents_;
    unsigned long threadId_;
    //MutexLock mutex_;
};
int createEventfd();
void handleNotify(void *);
}



#endif /* __EVENTLOOP_H */
