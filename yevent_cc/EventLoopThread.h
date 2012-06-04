/*
 * =====================================================================================
 *
 *       Filename:  EventLoopThread.h
 *
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  2012年05月28日 09时28分18秒
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  YOUR NAME (), 
 *        Company:  
 *
 * =====================================================================================
 */
#ifndef __EVENT_LOOP_THREAD_H
#define __EVENT_LOOP_THREAD_H
#include "EventLoop.h"
#include "Thread.h"
#include "MutexLock.h"
#include "ThreadCondition.h"
namespace yevent
{
    namespace thread {
        void *eventLoopThreadFunc(void *args);
    }
class EventLoopThread 
{
public:
    EventLoopThread();
    ~EventLoopThread();
    EventLoop* start();
    EventLoop *getLoop() {return pLoop_;}
    void quit();
private:
    friend void *thread::eventLoopThreadFunc(void *args);
    bool running_;
    EventLoop *pLoop_;
    MutexLock mutex_;
    ThreadCondition cond_;
    Thread thread_;
};
}
#endif
