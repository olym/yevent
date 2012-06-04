/*
 * =====================================================================================
 *
 *       Filename:  EventLoopThreadPool.h
 *
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  05/28/2012 11:27:19 PM
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  YOUR NAME (), 
 *        Company:  
 *
 * =====================================================================================
 */

#ifndef __EVENT_LOOP_THREAD_POOL_H
#define __EVENT_LOOP_THREAD_POOL_H
#include "EventLoopThread.h"
#include "EventLoop.h"
#include <vector>

namespace yevent {
class EventLoopThreadPool
{
    public:
        EventLoopThreadPool(EventLoop *loop);
        ~EventLoopThreadPool();
        
        void start(int numThreads);
        void quit();
        EventLoop *getNextLoop();

    private:
        EventLoop *baseLooP_;
        unsigned int numThreads_;
        std::vector<EventLoopThread *> threads_;
        std::vector<EventLoop *> threadLoops_;
        int nextLoopIndex_;
        bool isRunning_;
};
}
#endif
