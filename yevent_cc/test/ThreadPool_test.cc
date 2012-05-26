/*
 * =====================================================================================
 *
 *       Filename:  ThreadPool_test.cc
 *
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  05/27/2012 12:54:23 AM
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  Yin Zhongxing (olym), zhongxing.yin@cs2c.com.cn
 *        Company:  www.CS2C.com.cn
 *
 * =====================================================================================
 */
#include "../ThreadPool.h"
#include <stdio.h>
#include <unistd.h>
using namespace yevent;
void taskFunc(void *args)
{
    printf("taskFunc = %d\n", (int)args);
    sleep(10);
}
int main(int argc, char *argv[])
{
    ThreadPool pool;
    for (int i = 0; i < 5; i++) {
        Task task(taskFunc, (void *)i);
        pool.schedule(task);
    }
    pool.start(5);
    pause();
    return 0;
}
