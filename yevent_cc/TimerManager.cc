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
int TimerCompareFunc(void *val1, void *val2)
{
       TimerEvent *timer1 = static_cast<TimerEvent *>(val1); 
       TimerEvent *timer2 = static_cast<TimerEvent *>(val2); 

       return timer1->getWhen() > timer2->getWhen();
}
}

using namespace yevent;
TimerEvent::TimerEvent(EventLoop *loop, int fd, Timestamp when, double interval) :
    Event(loop, fd, EV_READ),
    id_(-1),
    when_(when), 
    interval_(interval),
    repeat_(interval > 0.0),
    isValid_(true)
{
}
void TimerEvent::handleEvent()
{
    uint64_t exp;
    int s = ::read(getFd(), &exp, sizeof(uint64_t));
    if (s != sizeof(uint64_t))
        fprintf(stderr, "%s: read error \n", __func__);

    //readCallback_(evReadArgs_);
}

TimerManager::TimerManager(EventLoop *loop):
    timerFd_(CreateTimerfd()), pLoop_(loop), 
    currentTimerId_(0), 
    minHeap_(new MinHeap<TimerEvent *>(TimerCompareFunc, NULL)) 
{
    minHeap_->init();
}

void TimerManager::handleTimerEvents()
{
    util::yeventLog(YEVENT_DEBUG, "%s", __func__);
    TimerEvent *timer = getNearestValidTimer();
    if (timer == NULL)
        return;
    std::cout << timer->getWhen().toString() << std::endl;
    std::cout << Timestamp::now().toString() << std::endl;
    while (timer->getWhen() < Timestamp::now()) {
        timer->handleRead();
        if (timer->isRepeat()) {
            minHeap_->pop();
            timer->setWhen(addTime(timer->getWhen(), timer->getInterval()));
            minHeap_->push(timer);
        } else {
            pLoop_->deleteEvent(timer);
            delete minHeap_->pop();
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
    TimerEvent *timer = minHeap_->top();
    if (timer == NULL)
        return NULL;
    while (!timer->isValid()) {
        delete minHeap_->pop();
        delete timer;

        if (minHeap_->size() == 0)
            break;
        timer = minHeap_->top();
        if (timer == NULL)
            return NULL;
    }

    return timer;
}

long TimerManager::addTimer(double timeout, double interval, TimerCallback cb, void *args)
{
    //Timestamp now = Timestamp::now();
    Timestamp when(static_cast<int>(interval)==0?timeout:interval);
    std::cout << when.toString() << std::endl;
    TimerEvent *timer = new TimerEvent(pLoop_, timerFd_, when, interval);
    timer->setReadCallback(cb, args);

    //MinHeapEntry *entry = new MinHeapEntry(static_cast<void *>(timer));
    minHeap_->push(timer);

    updateTimerEvent();

}
void TimerManager::timerfdSetTimer(TimerEvent *timer)
{
  struct itimerspec newValue;
  long sec, usec;
  bzero(&newValue, sizeof newValue);
  TimestampToTimeval(timer->getWhen(), &sec, &usec);
  newValue.it_value.tv_sec = sec;
  newValue.it_value.tv_nsec = usec * 1000;
  int ret = timerfd_settime(timerFd_, 0, &newValue, NULL);
  if (ret) {
      fprintf(stderr, "%s: timerfd_settime()", __func__);
  }
}
void TimerManager::updateTimerEvent()
{
    TimerEvent *timer;
    if (timer = getNearestValidTimer()) {
        timerfdSetTimer(timer);
        pLoop_->updateEvent(timer);
    }
}
void TimerManager::deleteTimer(TimerEvent *timer)
{
    assert(timer);

    for (int i = 0; i < minHeap_->size(); ++i) {
        if (minHeap_->getEntry(i) == timer ) {
            timer->setValid(false);
            updateTimerEvent();
        }
    }
}
