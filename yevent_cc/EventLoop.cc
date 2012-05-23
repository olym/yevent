/*
 * =====================================================================================
 *
 *       Filename:  EventLoop.cc
 *
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  04/25/2012 09:55:32 PM
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  Yin Zhongxing (olym), zhongxing.yin@cs2c.com.cn
 *        Company:  www.CS2C.com.cn
 *
 * =====================================================================================
 */
#include <sys/eventfd.h>
#include <stdio.h>
#include <vector>
#include <iostream>
#include "EventLoop.h"
#include "Event.h"
#include "TimerManager.h"
#include "Multiplexer.h"
#include "SignalEvent.h"

namespace yevent {
int createEventfd() 
{
    int notifyfd = eventfd(0, EFD_NONBLOCK | EFD_CLOEXEC);
    if (notifyfd < 0) {
        fprintf(stderr, "%s:Failed to create event fd\n", __func__);
        return -1;
    }

    return notifyfd;
}
void handleNotify(void *args)
{
    EventLoop *loop = static_cast<EventLoop *> (args);
    uint64_t one;
    int n = read(loop->notifyfd_, &one, sizeof(one));
    if (n != sizeof one) {
        fprintf(stderr, "%s: read error\n", __func__);
    }
    
    loop->isNotifyPending_ = 0;

}
}

using namespace yevent;

EventLoop::EventLoop()
    :stop_(false),
    notifyfd_(createEventfd()),
    isNotifyPending_(0),
    notifyEvent_(new Event(this, notifyfd_, EV_READ)),
    multiplexer_(NewMultiplexerImp(Multiplexer_EPOLL)),
    timerManager_(new TimerManager(this))
    //threadId_(Thread::tid())
{
}

void EventLoop::init()
{
    multiplexer_->initialize(this);

    notifyEvent_->setReadCallback(handleNotify, this);
    registerEvent(notifyEvent_.get());
}
void EventLoop::registerEvent(Event *event)
{
    if (registeredEvents_.find(event->getFd()) == registeredEvents_.end()) {
        if (multiplexer_->addEvent(event->getFd(), event->getEvent()) != -1)
            registeredEvents_[event->getFd()] = event;
    } else {
        //updateEvent(event);
        multiplexer_->updateEvent(event->getFd(), event->getEvent());
    }

}

void EventLoop::unregisterEvent(Event *ev)
{
    if (registeredEvents_.find(ev->getFd()) != registeredEvents_.end()) {
        deleteEvent(ev);
        registeredEvents_.erase(ev->getFd());
    }
}

void EventLoop::updateEvent(Event *event)
{
    if (registeredEvents_.find(event->getFd()) == registeredEvents_.end()) {
        if (multiplexer_->addEvent(event->getFd(), event->getEvent()) != -1)
            registeredEvents_[event->getFd()] = event;
    } else {
        multiplexer_->updateEvent(event->getFd(), event->getEvent());
    }
}

void EventLoop::deleteEvent(Event *event)
{
    multiplexer_->deleteEvent(event->getFd(), event->getEvent());
}

void EventLoop::dispatch()
{
    int numevents;
    stop_ = false;

    while (!stop_) {

        activeEvents_.clear();

        numevents = multiplexer_->dispatch(0);
        printf("%s: numevents = %d\n", __func__, numevents);

        for (std::vector<Event *>::iterator it = activeEvents_.begin();
                it != activeEvents_.end(); ++it) {
            (*it)->handleEvent();
        }
        timerManager_->handleTimerEvents();
        //runPendingFunctors();
    } 
}

Event* EventLoop::registerSignalEvent(int signo, EventCallback cb, void *arg)
{
    Event *ev = new SignalEvent(this, signo);
    ev->setReadCallback(cb, arg);
    
    registerEvent(ev);

    return ev;
}

void EventLoop::wakeup()
{
    uint64_t one = 1;
    //if the base already has a pending notify, we don't need to write any more
    if (!isNotifyPending_) {
        isNotifyPending_ = 1;
        ssize_t n = write(notifyfd_, &one, sizeof one);
        if (n != sizeof one) {
            fprintf(stderr, "%s:write %d bytes instead of 8", __func__, n);
        }
    }
}

long EventLoop::runAt(Timestamp &ts, TimerCallback cb, void *arg)
{
    return timerManager_->addTimer(ts, 0.0, cb, arg);
}

long EventLoop::runAfter(double timeout, TimerCallback cb, void *arg)
{
    std::cout << "runAfter : now time = Timestamp::now().toString() << std::endl;
    return timerManager_->addTimer(addTime(Timestamp::now(), timeout), 0.0, cb, arg);
}

long EventLoop::runEvery(double interval, TimerCallback cb, void *arg)
{
    return timerManager_->addTimer(addTime(Timestamp::now(), interval), interval, cb, arg);
}

void EventLoop::deleteTimer(long timerId)
{

}

