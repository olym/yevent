/*
 * =====================================================================================
 *
 *       Filename:  Thread.cc
 *
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  2012年05月24日 15时42分52秒
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  Yin Zhongxing (olym), zhongxing.yin@cs2c.com.cn
 *        Company:  www.CS2C.com.cn
 *
 * =====================================================================================
 */

#include <stdio.h>
#include <sys/types.h>
#include <sys/syscall.h>
#include <assert.h>
#include <string>
#include "Thread.h"
#include "Utility.h"

namespace yevent {
}

using namespace yevent;
using std::string;

Thread::Thread(ThreadFunc func, void *args, string name) :
    threadId_(-1),
    tid_(-1),
    threadFunc_(func),
    args_(args),
    name_(name),
    isRunning_(false)
{
}

Thread::~Thread()
{
}

void Thread::start()
{
    assert(!isRunning_);
    int ret = -1;
    ret = pthread_create(&threadId_, NULL, &startThread, this);
    if (ret != 0)
        fprintf(stderr, "pthread_create error\n");

}

int  Thread::join()
{
    assert(isRunning_);
    return pthread_join(threadId_, NULL);
}

void *Thread::startThread(void *args)
{
    Thread *thread = static_cast<Thread *>(args);
    thread->runInThread();
    return NULL; 
}

void *Thread::runInThread()
{
    isRunning_ = true;
    tid_ = util::gettid();
    threadFunc_(args_);
}
