/*
 * =====================================================================================
 *
 *       Filename:  EventLoopThreadPool.h
 *
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  04/26/2012 12:47:29 AM
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  YOUR NAME (), 
 *        Company:  
 *
 * =====================================================================================
 */
#ifndef __THREAD_POOL_H
#define __THREAD_POOL_H
#include <vector>
#include <deque>
#include "MutexLock.h"
#include "ThreadCondition.h"
#include "Thread.h"

namespace yevent
{
    class MutexLock;
    class ThreadCondition;
    class Thread;

    typedef void (*TaskFunc)(void *args);
    class Task {
        public:
            Task(TaskFunc taskFunc, void *args) : taskFunc_(taskFunc), args_(args) {}
            void runTask() { if (taskFunc_) taskFunc_(args_);}
        private:
            TaskFunc taskFunc_;
            void *args_;
    };
    class ThreadPool {
        public:
            ThreadPool();
            ~ThreadPool();

            void start(int numThreads);
            void stop();

            void schedule(const Task& f); // add some tasks to the pool
            bool isRunning_() {return running_;}

        private:
            static void *excuteThread(void *);  //if there is task in the queue, fetch one and excute the task.
            Task fetchTask(); // fetch task from task queue;

            MutexLock mutex_;
            ThreadCondition cond_;
            std::vector<Thread *> threads_;
            std::deque<Task> queue_;
            int numThreads_;
            bool running_;
    };
}

#endif
