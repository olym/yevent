/*
 * =====================================================================================
 *
 *       Filename:  ThreadCondition.h
 *
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  05/26/2012 10:15:09 AM
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  YOUR NAME (), 
 *        Company:  
 *
 * =====================================================================================
 */

#include "../ThreadCondition.h"
#include "../Thread.h"
#include "../MutexLock.h"
#include <iostream>
using namespace yevent;
using namespace std;
struct Instance {
    ThreadCondition *cond;
    MutexLock *mutex;
};
void *foo(void *obj)
{
    Instance *inst = static_cast<Instance *>(obj);
    MutexLockGuard lock(*(inst->mutex));
    cout << "thread: get lock" << endl;
    sleep(5);
    cout << "thread: notify" << endl;
    inst->cond->notify();
}


int main(int argc, char *argv[])
{
    MutexLock mutex;
    ThreadCondition condition(mutex);
    Instance  inst;
    inst.cond = &condition;
    inst.mutex = &mutex;

    Thread thread(foo, &inst, "");
    thread.start();

    MutexLockGuard lock(mutex);
    cout << "main: waiting" << endl;
    condition.wait();
    cout << "main: wait up" << endl;

    return 0;
}
