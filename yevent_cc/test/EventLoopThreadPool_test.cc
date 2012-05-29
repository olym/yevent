/*
 * =====================================================================================
 *
 *       Filename:  EventLoopThreadPool_test.cc
 *
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  2012年05月29日 14时01分48秒
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  Yin Zhongxing (olym), zhongxing.yin@cs2c.com.cn
 *        Company:  www.CS2C.com.cn
 *
 * =====================================================================================
 */

#include "../EventLoopThreadPool.h"
#include <stdio.h>
using namespace yevent;
void func1(void *args)
{
    printf("%s in first loop is running\n", __func__); 
}
void func2(void *args)
{
    printf("%s in second loop is running\n", __func__); 
}
int main(int argc, char *argv[])
{
    EventLoop baseLoop;
    EventLoop *loop;
    baseLoop.init();

    EventLoopThreadPool pool(&baseLoop);
    pool.start(2);

    loop = pool.getNextLoop();
    Task task1(func1, loop);
    loop->runInLoop(task1);

    loop = pool.getNextLoop();
    Task task2(func2, loop);
    loop->runInLoop(task2);

    baseLoop.dispatch();
    return 0;
}
