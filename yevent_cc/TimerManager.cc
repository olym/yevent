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
#include <sys/timerfd.h>
#include "EventLoop.h"
#include "Event.h"
#include "TimerManager.h"
#include "MinHeap.h"
#include "Timestamp.h"

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

void TimerEvent::handleEvent()
{
    uint64_t exp;
    int s = ::read(getFd(), &exp, sizeof(uint64_t));
    if (s != sizeof(uint64_t))
        fprintf(stderr, "%s: read error \n", __func__);

    //readCallback_(evReadArgs_);
}

void TimerManager::handleTimerEvents()
{
    TimerEvent *timer = getNearestValidTimer();
    while (timer->getWhen() < Timestamp::now()) {
        timer->handleRead();
        if (timer->isRepeat()) {
            MinHeapEntry *entry = minHeap_->pop();
            timer->setWhen(addTime(timer->getWhen(), timer->getInterval()));
            minHeap_->push(entry);
        } else {
            delete minHeap_->pop();
            delete timer;
        }
        timer = getNearestValidTimer();
    }

    updateTimerEvent();
}

TimerEvent *TimerManager::getNearestValidTimer()
{
    TimerEvent *timer = static_cast<TimerEvent *>(minHeap_->top()->data);
    if (timer == NULL)
        return NULL;
    while (!timer->isValid()) {
        delete minHeap_->pop();
        delete timer;

        timer = static_cast<TimerEvent *>(minHeap_->top()->data);
        if (timer == NULL)
            return NULL;
    }

    return timer;
}

long TimerManager::addTimer(Timestamp when, double interval, TimerCallback cb, void *args)
{
    TimerEvent *timer = new TimerEvent(pLoop_, timerFd_, when, interval);
    timer->setReadCallback(cb, args);

    MinHeapEntry *entry = new MinHeapEntry(static_cast<void *>(timer));
    minHeap_->push(entry);

    updateTimerEvent();

}
void TimerManager::updateTimerEvent()
{
    TimerEvent *timer;
    if (timer = getNearestValidTimer())
        pLoop_->updateEvent(timer);
}
void TimerManager::deleteTimer(TimerEvent *timer)
{
    assert(timer);

    for (int i = 0; i < minHeap_->size(); ++i) {
        if (static_cast<TimerEvent *>(minHeap_->getEntry(i)->data) == timer ) {
            timer->setValid(false);
            updateTimerEvent();
        }
    }
}
