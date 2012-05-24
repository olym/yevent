/*
 * =====================================================================================
 *
 *       Filename:  MutexLock.h
 *
 *    Description:
 *
 *        Version:  1.0
 *        Created:  04/26/2012 12:48:00 AM
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  YOUR NAME (),
 *        Company:
 *
 * =====================================================================================
 */
#ifndef _MUTEX_LOCK_H
#define _MUTEX_LOCK_H

#include <assert.h>
#include <pthread.h>

namespace yevent
{
namespace util
{
    pid_t gettid();
}
class MutexLock
{
public:
    MutexLock()
        : holder_(0) {
        pthread_mutex_init(&mutex_, NULL);
    }

    ~MutexLock() {
        assert(holder_ == 0);
        pthread_mutex_destroy(&mutex_);
    }

    bool isLockedByThisThread() {
        return holder_ == util::gettid();
    }

    void assertLocked() {
        assert(isLockedByThisThread());
    }


    void lock() {
        pthread_mutex_lock(&mutex_);
        holder_ = util::gettid(); 
    }

    void unlock() {
        holder_ = 0;
        pthread_mutex_unlock(&mutex_);
    }

    pthread_mutex_t* getPthreadMutex() {
        return &mutex_;
    }

private:

    pthread_mutex_t mutex_;
    pid_t holder_;
};
class MutexLockGuard
{
public:
    explicit MutexLockGuard(MutexLock& mutex) : mutex_(mutex) {
        mutex_.lock();
    }

    ~MutexLockGuard() {
        mutex_.unlock();
    }

private:

    MutexLock& mutex_;
};
}

#endif
