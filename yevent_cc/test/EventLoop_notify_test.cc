/*
 * =====================================================================================
 *
 *       Filename:  EventLoop.cc
 *
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  2012年05月23日 11时02分29秒
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  Yin Zhongxing (olym), zhongxing.yin@cs2c.com.cn
 *        Company:  www.CS2C.com.cn
 *
 * =====================================================================================
 */

#include "../EventLoop.h"

using namespace yevent;
void *notify(void *args)
{
    EventLoop *loop = (EventLoop *)args;

    sleep(2);

    loop->wakeup();
    pthread_detach(pthread_self());
    return NULL;
}

int main(int argc, char *argv[])
{
    EventLoop loop;
    pthread_t thread;
    loop.init();
    pthread_create(&thread, NULL, notify, &loop);
    loop.dispatch();
    return 0;
}
