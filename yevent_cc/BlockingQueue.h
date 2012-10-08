/*
 * =====================================================================================
 *
 *       Filename:  BlocingQueue.h
 *
 *    Description:
 *
 *        Version:  1.0
 *        Created:  07/27/2011 12:06:50 AM
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  YOUR NAME (),
 *        Company:
 *
 * =====================================================================================
 */

#ifndef _BLOCKINGQUEUE_H
#define _BLOCKINGQUEUE_H

#include <deque>
#include <assert.h>

#include "ThreadCondition.h"
#include "MutexLock.h"

template<typename T>
class BlockingQueue
{
public:
    BlockingQueue()
        : mutex_(),
          condition_variable_(mutex_),
          queue_() {
    }
    void push(T const& data) {
        MutexLockGuard<MutexLock> lock(mutex_);
        queue_.push_back(data);
        condition_variable_.notify();
    }

    bool try_pop(T& popped_value) {
        MutexLockGuard<MutexLock> lock(mutex_);
        //if (empty())    // wrong not recursion mutex
        if (queue_.empty())
            return false;
        popped_value = queue_.front();
        queue_.pop_front();
        return true;
    }

    T wait_pop() {
        MutexLockGuard<MutexLock> lock(mutex_);
        while (queue_.empty())
            condition_variable_.wait();
        assert(!queue_.empty());
        T popped_value(queue_.front());
        queue_.pop_front();
        return popped_value;
    }

    bool empty() const {
        MutexLockGuard<MutexLock> lock(mutex_);
        return queue_.empty();
    }

    size_t size() const {
        MutexLockGuard<MutexLock> lock(mutex_);
        return queue_.size();
    }
private:
    BlocingQueue(const BlocingQueue&);
    void operator=(const BlocingQueue&);
    mutable MutexLock mutex_;
    ThreadCondition         condition_variable_;
    std::deque<T>     queue_;
};

#endif
