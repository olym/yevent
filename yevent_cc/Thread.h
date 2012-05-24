/*
 * =====================================================================================
 *
 *       Filename:  Thread.h
 *
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  04/26/2012 12:47:52 AM
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  YOUR NAME (), 
 *        Company:  
 *
 * =====================================================================================
 */
#ifndef __THREAD_H
#define __THREAD_H

#include <pthread.h>
#include <string>

namespace yevent
{
    namespace util {
        pid_t gettid();
    }
    
    typedef void *(*ThreadFunc)(void *);

    class Thread 
    {
        public:
            Thread(ThreadFunc, void *, std::string);
            ~Thread();

            void start();
            int join();

            bool isRunning() { return isRunning_;}
            pid_t getTid() {return tid_;}
            const std::string& getName() {return name_;}

        private:
            static void *startThread(void *args);
            void *runInThread();
            pthread_t threadId_;
            pid_t tid_;
            ThreadFunc threadFunc_;
            void *args_;
            std::string name_;
            bool isRunning_;
    };
}
#endif 
