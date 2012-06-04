/*
 * =====================================================================================
 *
 *       Filename:  EventLoop_timer_test.cc
 *
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  2012年05月23日 14时20分30秒
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  Yin Zhongxing (olym), zhongxing.yin@cs2c.com.cn
 *        Company:  www.CS2C.com.cn
 *
 * =====================================================================================
 */

#include <stdio.h>
#include "../EventLoop.h"

using namespace yevent;

void timerCallback(void *args)
{
    int val = (int)args;
    printf("%s val = %d\n", __func__, val);
}
int main(int argc, char *argv[])
{
    EventLoop loop;
    loop.init();
    loop.registerTimerEvent(5.5, 0.0, timerCallback, (void *)3);
    loop.registerTimerEvent(5.5, 0.0, timerCallback, (void *)4);
    loop.registerTimerEvent(2.5, 0.0, timerCallback, (void *)1);
    loop.registerTimerEvent(0.0, 3.3, timerCallback, (void *)2);
    loop.dispatch();
    return 0;
}
