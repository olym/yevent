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
    loop.runAfter(2.0, timerCallback, (void *)1);
    loop.runAfter(4.0, timerCallback, (void *)2);
    loop.runAfter(5.5, timerCallback, (void *)3);
    loop.dispatch();
    return 0;
}
