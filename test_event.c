/*
 * =====================================================================================
 *
 *       Filename:  test_event.c
 *
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  02/13/2012 11:26:25 PM
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
#include <time.h>
#include "event.h"
void signal_fn(int fd, short mask, void* args)
{
    printf("singal\n");
}
void event_thread(void *args)
{
    struct event_base *base;
    struct event ev;
    struct timeval tv;
    tv.tv_sec = 2;
    tv.tv_usec = 0;
    base = event_base_new();
    event_assign(&ev, base, -1, EV_TIMEOUT | EV_PERSIST, signal_fn, NULL);
    event_add(&ev, &tv);
    event_base_dispatch(base); 
    printf("=============\n");
}

int main(int argc, char *argv[])
{
    struct event_base *base;
    pthread_t pid1;
    pthread_t pid2;
    //base = event_init();
    pthread_create(&pid1, NULL, event_thread, NULL); 
    pthread_create(&pid2, NULL, event_thread, NULL); 
    sleep(10);
    return 0;
}
