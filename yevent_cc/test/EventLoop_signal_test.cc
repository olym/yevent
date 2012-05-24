/*
 * =====================================================================================
 *
 *       Filename:  EventLoop_signal_test.cc
 *
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  05/23/2012 11:41:46 PM
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  Yin Zhongxing (olym), zhongxing.yin@cs2c.com.cn
 *        Company:  www.CS2C.com.cn
 *
 * =====================================================================================
 */
#include <stdio.h>
#include <signal.h>
#include "../EventLoop.h"
using namespace yevent;
void signalCallback(void *args)
{
    printf("SIGINI\n");
}
int main(int argc, char *argv[])
{
    EventLoop loop;
    loop.init();
    
    loop.registerSignalEvent(SIGINT, signalCallback, NULL);

    loop.dispatch(); 
    return 0;
}
