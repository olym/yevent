/*
 * =====================================================================================
 *
 *       Filename:  EventLoopThreadPool.c
 *
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  2012年05月29日 11时08分00秒
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  Yin Zhongxing (olym), zhongxing.yin@cs2c.com.cn
 *        Company:  www.CS2C.com.cn
 *
 * =====================================================================================
 */
#include "EventLoopThreadPool.h"

using namespace yevent;
using std::vector;

EventLoopThreadPool::EventLoopThreadPool(EventLoop *loop) : 
            baseLooP_(loop), 
            numThreads_(0), 
            nextLoopIndex_(0),
            isRunning_(false)
{
}

EventLoopThreadPool::~EventLoopThreadPool()
{
    if (isRunning_)
        quit();
}
void EventLoopThreadPool::start(int numThreads)
{
    EventLoopThread *thread;
    EventLoop *loop;
    if (isRunning_ == false) {
        util::yeventLog(YEVENT_DEBUG, "EventLoopThreadPool::%d thread start", numThreads);
        for (int i = 0; i < numThreads; i++) {
            thread = new EventLoopThread;
            threads_.push_back(thread);
            loop = thread->start();
            threadLoops_.push_back(loop);
        }
        numThreads_ = numThreads;
    }
    isRunning_ = true;
}
void EventLoopThreadPool::quit()
{
    for (int i = 0; i < numThreads_; i++) {
        threads_[i]->quit();
        delete threads_[i];
    }
    threads_.clear();
    threadLoops_.clear();
    isRunning_ = false;
}

EventLoop *EventLoopThreadPool::getNextLoop()
{
    if (numThreads_ == 0)
        return baseLooP_;
    else {
        if (nextLoopIndex_ == numThreads_)
            nextLoopIndex_ = 0;
        EventLoop *loop = threadLoops_[nextLoopIndex_];
        nextLoopIndex_ += 1;
        return loop;
    }
}
