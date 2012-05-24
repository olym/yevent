/*
 * =====================================================================================
 *
 *       Filename:  MutexLock_test.h
 *
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  05/25/2012 12:18:52 AM
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  YOUR NAME (), 
 *        Company:  
 *
 * =====================================================================================
 */

#include <stdio.h>
#include "../MutexLock.h"
#include "../Thread.h"

using namespace yevent;
struct Some{
    MutexLock mutex;
    int val;   
};
void *threadFunc(void *args)
{
    Some *psome = static_cast<Some *>(args);
    while (1)
    {
        MutexLockGuard lock(psome->mutex);
        printf("thread val = %d\n", psome->val);
        psome->val++;
    }
}
int main(int argc, char *argv[])
{
    Some some;
    some.val = 1;
    Thread thread(threadFunc, &some, "abc");
    thread.start();
    while(1)
    {
        MutexLockGuard lock(some.mutex);
        printf("main thread val = %d\n", some.val);
        some.val++;
    }
    return 0;
}
