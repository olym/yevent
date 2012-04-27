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
    timerManager_(new TimerEvent_),
    threadId_(Thread::tid());
{
    notifyEvent_->setReadCallback(EventCallback cb, NULL);
    registerEvent(&notifyEvent_);
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

EventLoop::registerSignalEvent(int signo, EventCallback cb, void *arg)
{
    Event *ev = new SignalEvent(this, signo, EV_READ);
    ev->setReadCallback(cb, arg);
    
    registerEvent(ev);
}


