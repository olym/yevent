/*
 * =====================================================================================
 *
 *       Filename:  TimerManager.cc
 *
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  04/27/2012 03:51:15 PM
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  Yin Zhongxing (olym), olym.yin@gmail.com
 *        Company:  
 *
 * =====================================================================================
 */
#include <stdio.h>
#include <stdlib.h>
#include "TimerManager.h"

using namespace yevent;

int CreateTimerfd()
{
    int timerfd = timerfd_create(CLOCK_MONOTONIC, TFD_NONBLOCK | TFD_CLOEXEC);

    if (timerfd < 0) {
        fprintf(stderr, "failed in timerfd_create\n");
        return -1;
    }

    return timerfd;
}

virtual void TimerEvent::handleEvent()
{
    uint64_t exp;
    int s = ::read(signalfd, &exp, sizeof(uint64_t));
    if (s != sizeof(uint64_t))
        fprintf(stderr, "%s: read error \n", __func__);

    Timer *timer = getTimer(currentTimerId_);

    timer->timerCb();

}

//FIXME 
TimerEvent *TimerManager::getNearestValidTimer()
{
    TimerEvent *timer = static_cast<TimerEvent *>(minHeap_->top().data);
    if (timer == NULL)
        return NULL;
    while (!timer->isValid()) {
        minHeap_->pop();
        timer = static_cast<TimerEvent *>(minHeap_->top().data);
        if (timer == NULL)
            return NULL;
    }

    return timer;
}

long TimerManager::addTimer(Timestamp when, double interval, TimerCallback cb, void *args)
{
    TimerEvent *timer = new TimerEvent(pLoop_, timerFd_, when, interval);
    timer->setReadCallback(cb, args);

    MinHeapEntry *entry = new MinHeapEntry(static_cast<void *>timer);
    minHeap_->push(entry);

    updateTimer();

}
void TimerManager::updateTimer()
{
    if (getNearestValidTimer())
        pLoop_->updateEvent(timer);
}
//FIXME
long TimerManager::deleteTimer(TimerEvent *timer)
{
    assert(timer);

    if (timer == currentTimer_) {
        if ( minHeap_->size() != 0) {
            currentTimer_ == getNearestValidTimer();
            if (currentTimer_ != NULL)
                currentTimer_->updateEvent();
        } esle {
            currentTimer_->deleteEvent();
        }
    } else {
        for (int i = 0; i < minHeap_->size(); ++i) {
            if (static_cast<TimerEvent *>(minHeap_[i]->data) == timer ) {
                timer->setValid(false);
            }
        }
    }
}
