/*
 * =====================================================================================
 *
 *       Filename:  ThreadPool.cc
 *
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  05/26/2012 10:26:53 PM
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  Yin Zhongxing (olym), zhongxing.yin@cs2c.com.cn
 *        Company:  www.CS2C.com.cn
 *
 * =====================================================================================
 */
#include "ThreadPool.h"
#include <stdio.h>
#include <stdlib.h>
#include <vector>
#include <deque>
#include <pthread.h>
#include <assert.h>

using namespace yevent;
using std::vector;
using std::deque;

ThreadPool::ThreadPool() :
    mutex_(),
    cond_(mutex_),
    numThreads_(0),
    running_(false)
{
}

ThreadPool::~ThreadPool()
{
    stop();
}

void ThreadPool::start(int numThreads)
{
    assert(threads_.empty());
    threads_.reserve(numThreads);
    numThreads_ = numThreads;

    running_ = true;
    for (int i = 0; i < numThreads_; i++) {
        char id[8];
        sprintf(id, "%d", i);
        threads_.push_back(new Thread(&ThreadPool::excuteThread, this, id));
        threads_[i]->start();
    }

}

void ThreadPool::stop()
{
    running_ = false;
    cond_.notifyAll();
    for (int i = 0; i < numThreads_; i++) {
        threads_[i]->join();
        delete threads_[i];
    }
    threads_.clear();
}

void ThreadPool::schedule(const Task &t)
{
    MutexLockGuard lock(mutex_);
    queue_.push_back(t);
    cond_.notify();
}

Task ThreadPool::fetchTask()
{
    MutexLockGuard lock(mutex_);
    while (queue_.empty()) {
        cond_.wait();
    }
    Task task(NULL, NULL); 
    if (!queue_.empty()) {
        task = queue_.front();
        queue_.pop_front();
    }

    return task;
}

void* ThreadPool::excuteThread(void *args)
{
    ThreadPool *pool = static_cast<ThreadPool *>(args);
    try {
        while(pool->isRunning_()) {
            Task task(pool->fetchTask());
            task.runTask();
        }
    } catch (const std::exception& ex) {
        fprintf(stderr, "exception caught in ThreadPool\n");
        fprintf(stderr, "reason: %s\n", ex.what());
        abort();
    } catch (...) {
        fprintf(stderr, "unknown exception caught in ThreadPool\n");
        abort();
    }
}
