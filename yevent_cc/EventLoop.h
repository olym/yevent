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
    long runAt(double expire, TimerCallback cb, void *privdata);
    long runEvery(double interval, TimerCallback cb, void *privdata);
    long runAfter(double delay, TimerCallback cb, void *privdata);
    void deleteTimer(long timerId);
    int registerSignalEvent(int signal, SignalCallback cb, void *privdata);
    void unregisterSignalEvent();
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
    boost::scoped_ptr<SignalManager> SignalManager_;
    std::map<int, Event*> registeredEvents_;
    std::vector<Event *> activeEvents_;
    unsigned long threadId_;
    MutexLock mutex_;
};
}

EventLoop::EventLoop()
    :stop_(false),
    notifyfd_(createEventfd()),
    notifyEvent_(new Event(this, notifyfd_, EV_READ)),
    mutiplexer_(NewMultiplexerImp(Multiplexer_EPOLL)),
    timerManager_(new TimerManager),
    threadId_(Thread::tid());
{
    notifyEvent_->setReadCallback(EventCallback cb, NULL);
    registerEvent(&notifyEvent_);
}

void registerSignalEvent(int signal, SignalCallback cb, void *privdata)
{
    int signalfd = signalfd();
    Event *ev = new Event(this, signalfd, EV_READ);

}

void EventLoop::registerEvent(Event *event)
{
    if (registeredEvents_.find(event->fd_) == registeredEvents_.end()) {
        if (mutiplexer_->addEvent(event->fd_, event->event_) != -1)
            registeredEvents_[event->fd_] = event;
    } else {
        updateEvent(event);
    }

}
void EventLoop::updateEvent(Event *event)
{
    mutiplexer_->updateEvent(event->fd, event->event_);
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
            //Event *ev = *it;
            //int rfired = 0;

            //if (ev->mask & EV_READ) {
            //    rfired = 1;
            //    ev->readCallback(this, fd, ev->privdata);
            //}
            //if (ev->mask & EV_WRITE) {
            //    if (!rfired || ev->wfileProc != ev->rfileProc)
            //        ev->writeCallback(this, fd, ev->privdata);
            //}
        }
        runPendingFunctors();
    } 
}

#endif /* __EVENTLOOP_H */
