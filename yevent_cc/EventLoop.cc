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
#include "MutexLock.h"
#include "EventLoop.h"
#include "Event.h"
#include "TimerManager.h"
#include "Multiplexer.h"
#include "SignalEvent.h"

/*EventLoop多线程使用说明：EventLoop很多情况下是用在多线程下的，所以如果在另外一个线程给loop所在线程添加事件的话，EvenLoop的大部分成员函数都需要加锁来保证数据的一致性，这样会造成锁的使用频率会很高，为了避免这种情况，可以将添加事件的任务放到该loop所在线程去执行。
实现这个功能需要在loop中增加执行deferred function的功能，这样只需将要执行的函数放到deferred function queue中，然后通知loop去执行即可，这样我们只需维护保护deferred queue的锁就可以了
*/

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
    timerManager_(new TimerManager(this)),
    threadId_(util::gettid()),
    mutex_()
{
}

EventLoop::~EventLoop()
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

void EventLoop::deleteEvent(Event *ev)
{
    if (registeredEvents_.find(ev->getFd()) != registeredEvents_.end()) {
        multiplexer_->deleteEvent(ev->getFd(), ev->getEvent());
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
        runDeferredTasks();
    } 
}

long EventLoop::registerTimerEvent(double timeout, double interval, TimerCallback cb, void *args)
{
    return timerManager_->addTimer(addTime(Timestamp::now(), interval==0.0?timeout:interval), interval, cb, args);
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
void EventLoop::breakLoop()
{
    stop_ = true;
    if (!isInLoopThread())
        wakeup();
}
void EventLoop::runDeferredTasks()
{
    MutexLockGuard lock(mutex_);
    for(std::vector<Task>::iterator it = deferredQueue_.begin();
            it != deferredQueue_.end(); it++)
        (*it).runTask();
    deferredQueue_.clear();
}
void EventLoop::runInLoop(Task task)
{
    if (isInLoopThread())
        task.runTask();
    else {
        {
        MutexLockGuard lock(mutex_);
        deferredQueue_.push_back(task);
        }
        wakeup();
    }
}

void EventLoop::deleteTimer(long timerId)
{

}

bool EventLoop::isInLoopThread()
{
    return threadId_ == util::gettid();
}

