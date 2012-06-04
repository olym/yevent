/*
 * =====================================================================================
 *
 *       Filename:  EventLoopThread.cc
 *
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  2012年05月28日 09时39分04秒
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  Yin Zhongxing (olym), zhongxing.yin@cs2c.com.cn
 *        Company:  www.CS2C.com.cn
 *
 * =====================================================================================
 */

#include "EventLoopThread.h"

using namespace yevent;

EventLoopThread::EventLoopThread() :
    running_(false),
    pLoop_(NULL),
    mutex_(),
    cond_(mutex_),
    thread_(&thread::eventLoopThreadFunc, this, "event loop thread")
{
}
EventLoopThread::~EventLoopThread()
{
    if (running_ == true)
        quit();
}
EventLoop* EventLoopThread::start()
{
    running_ = true;
    thread_.start(); 
    MutexLockGuard lock(mutex_);
    while (!pLoop_) {
        cond_.wait();
    }

    return pLoop_;
}
void EventLoopThread::quit()
{
    pLoop_->breakLoop();
    thread_.join();
    running_ = false;
}

namespace  yevent {
namespace thread{
void *eventLoopThreadFunc(void *args)
{
    EventLoopThread *elt = static_cast<EventLoopThread *>(args);
    EventLoop loop;
    loop.init();
    {
        MutexLockGuard lock(elt->mutex_);
        elt->pLoop_ = &loop;
        elt->cond_.notify();
    }
    loop.dispatch();

    return NULL;
}
}
}


