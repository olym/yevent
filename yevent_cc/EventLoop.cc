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
#include "EventLoop.h"
#include "Event.h"
#include "TimerManager.h"
#include "Multiplexer.h"
#include "SignalEvent.h"

using namespace yevent;

int createEventfd() 
{
    int notifyfd = eventfd(0, EFD_NONBLOCK | EFD_CLOEXEC);
    if (notifyfd < 0) {
        fprintf(stderr, "%s:Failed to create event fd\n", __func__);
        return -1;
    }

    return notifyfd;
}

EventLoop::EventLoop()
    :stop_(false),
    notifyfd_(createEventfd()),
    notifyEvent_(new Event(this, notifyfd_, EV_READ)),
    multiplexer_(NewMultiplexerImp(Multiplexer_EPOLL)),
    timerManager_(new TimerManager(this))
    //threadId_(Thread::tid())
{
    notifyEvent_->setReadCallback(handlerNotify, NULL);
    registerEvent(notifyEvent_.get());
}

void EventLoop::registerEvent(Event *event)
{
    if (registeredEvents_.find(event->getFd()) == registeredEvents_.end()) {
        if (multiplexer_->addEvent(event->getFd(), event->getEvent()) != -1)
            registeredEvents_[event->getFd()] = event;
    } else {
        updateEvent(event);
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
    multiplexer_->updateEvent(event->getFd(), event->getEvent());
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

long EventLoop::runAt(Timestamp &ts, TimerCallback cb, void *arg)
{
    return timerManager_->addTimer(ts, 0.0, cb, arg);
}

long EventLoop::runAfter(double timeout, TimerCallback cb, void *arg)
{
    return timerManager_->addTimer(addTime(Timestamp::now(), timeout), 0.0, cb, arg);
}

long EventLoop::runEvery(double interval, TimerCallback cb, void *arg)
{
    return timerManager_->addTimer(addTime(Timestamp::now(), interval), interval, cb, arg);
}

void EventLoop::deleteTimer(long timerId)
{

}

