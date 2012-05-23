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
#include <strings.h>
#include <sys/timerfd.h>
#include <iostream>
#include "EventLoop.h"
#include "Event.h"
#include "TimerManager.h"
#include "MinHeap.h"
#include "Timestamp.h"

namespace yevent {
int CreateTimerfd()
{
    int timerfd = timerfd_create(CLOCK_MONOTONIC, TFD_NONBLOCK | TFD_CLOEXEC);

    if (timerfd < 0) {
        fprintf(stderr, "failed in timerfd_create\n");
        return -1;
    }

    return timerfd;
}
}

using namespace yevent;
void TimerEvent::handleEvent()
{
    printf("%s\n", __func__);
    uint64_t exp;
    int s = ::read(getFd(), &exp, sizeof(uint64_t));
    if (s != sizeof(uint64_t))
        fprintf(stderr, "%s: read error \n", __func__);

    //readCallback_(evReadArgs_);
}

void TimerManager::handleTimerEvents()
{
    TimerEvent *timer = getNearestValidTimer();
    if (timer == NULL)
        return;
    std::cout << timer->getWhen().toString() << std::endl;
    std::cout << Timestamp::now().toString() << std::endl;
    while (timer->getWhen() < Timestamp::now()) {
        timer->handleRead();
        if (timer->isRepeat()) {
            MinHeapEntry *entry = minHeap_->pop();
            timer->setWhen(addTime(timer->getWhen(), timer->getInterval()));
            minHeap_->push(entry);
        } else {
            pLoop_->unregisterEvent(timer);
            delete minHeap_->pop();
            delete timer;
        }
        timer = getNearestValidTimer();
        if (timer == NULL)
            break;
    }

    updateTimerEvent();
}

TimerEvent *TimerManager::getNearestValidTimer()
{
    if (minHeap_->size() == 0)
        return NULL;
    TimerEvent *timer = static_cast<TimerEvent *>(minHeap_->top()->data);
    if (timer == NULL)
        return NULL;
    while (!timer->isValid()) {
        delete minHeap_->pop();
        delete timer;

        if (minHeap_->size() == 0)
            break;
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

    std::cout << "TimerManager::addTimer :" << when.toString() << std::endl;
    updateTimerEvent();

}
void TimerManager::resetTimer(TimerEvent *timer)
{
  struct itimerspec newValue;
  long sec, usec;
  bzero(&newValue, sizeof newValue);
  TimestampToTimeval(timer->getWhen(), &sec, &usec);
  newValue.it_value.tv_sec = sec;
  newValue.it_value.tv_nsec = usec * 1000;
  printf("%s: sec = %d usec = %d\n", __func__, sec, usec);
  int ret = timerfd_settime(timerFd_, 0, &newValue, NULL);
  if (ret) {
      fprintf(stderr, "%s: timerfd_settime()", __func__);
  }
}
void TimerManager::updateTimerEvent()
{
    TimerEvent *timer;
    if (timer = getNearestValidTimer()) {
        printf("%s\n", __func__);
        resetTimer(timer);
        pLoop_->updateEvent(timer);
    }
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
