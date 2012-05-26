/*
 * =====================================================================================
 *
 *       Filename:  ThreadCondition.h
 *
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  2012年05月24日 13时55分27秒
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  YOUR NAME (), 
 *        Company:  
 *
 * =====================================================================================
 */
#ifndef __THREAD_CONDITION_H
#define __THREAD_CONDITION_H

#include <pthread.h>
#include "MutexLock.h"

namespace yevent
{
    class ThreadCondition
    {
        public:
            explicit ThreadCondition(MutexLock& mutex) : mutex_(mutex)
            {
                pthread_cond_init(&pcond_, NULL);
            }

            ~ThreadCondition()
            {
                pthread_cond_destroy(&pcond_);
            }

            void wait()
            {
                pthread_cond_wait(&pcond_, mutex_.getPthreadMutex());
            }

            void notify()
            {
                pthread_cond_signal(&pcond_);
            }

            void notifyAll()
            {
                pthread_cond_broadcast(&pcond_);
            }

        private:
            MutexLock& mutex_;
            pthread_cond_t pcond_;
    };
}
#endif 
