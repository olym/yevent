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
using namespace yevent;

EventLoop::EventLoop()
    :stop_(false),
    notifyfd_(createEventfd()),
    notifyEvent_(new Event(this, notifyfd_, EV_READ)),
    mutiplexer_(NewMultiplexerImp(Multiplexer_EPOLL)),
    timerManager_(new TimerManager(this)),
    threadId_(Thread::tid());
{
    notifyEvent_->setReadCallback(EventCallback cb, NULL);
    registerEvent(&notifyEvent_);
}

void EventLoop::registerEvent(Event *event)
{
    if (registeredEvents_.find(event->getFd()) == registeredEvents_.end()) {
        if (mutiplexer_->addEvent(event->getFd(), event->getEvent()) != -1)
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
    mutiplexer_->updateEvent(event->getFd(), event->getEvent());
}
void EventLoop::deleteEvent(Event *event)
{
    mutiplexer_->deleteEvent(event->getFd(), event->getEvent());
}

void EventLoop::dispatch()
{
    int numevents;
    stop_ = false;

    while (!stop_) {

        activeEvents_.clear();

        numevents = mutiplexer_->dispatch();

        for (std::vector<Event *>::iterator it = activeEvents_.begin();
                it != activeEvents_.end(); ++iterator) {
            (*it)->handleEvent();
        }
        timerManager_->handleExpiredTimer();
        runPendingFunctors();
    } 
}

Event* EventLoop::registerSignalEvent(int signo, EventCallback cb, void *arg)
{
    Event *ev = new SignalEvent(this, signo, EV_READ);
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

