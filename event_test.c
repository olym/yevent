/*
 * =====================================================================================
 *
 *       Filename:  event_test.c
 *
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  02/14/2012 02:07:01 PM
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  Yin Zhongxing (olym), zhongxing.yin@cs2c.com.cn
 *        Company:  www.CS2C.com.cn
 *
 * =====================================================================================
 */

#include <stdio.h>
#include <pthread.h>
#include <sys/eventfd.h>
#include <signal.h>

#include "event.h"
void eventfd_fn(int fd, short mark, void *args)
{
     int one = 0;
     int r;
     r = read(fd, &one, sizeof one);
     printf("eventfd_fn fd=%d r = %d val = %d\n", fd, r, one);
}

void* thread_event(void *args)
{
    struct event_base *base;
    struct event ev;
    pthread_t tid;
    int fd;
    base = event_init();
    fd = eventfd(0, EFD_NONBLOCK | EFD_CLOEXEC);
    printf("thread_event fd = %d\n", fd);
    event_assign(&ev, base, fd, EV_READ | EV_PERSIST, eventfd_fn, NULL);
    event_add(&ev, NULL);
    //pthread_create(&tid, NULL, thread_event, NULL);
    event_base_dispatch(base); 
    printf("================11111111\n");
}

int main(int argc, const char *argv[])
{
    struct event_base *base;
    struct event ev;
    pthread_t tid;
    int fd;

    base = event_init();
    fd = eventfd(0, EFD_NONBLOCK | EFD_CLOEXEC);
    printf("main fd = %d\n", fd);
    //event_assign(&ev, base, SIGQUIT, EV_SIGNAL | EV_PERSIST, eventfd_fn, NULL);
    event_assign(&ev, base, fd, EV_READ | EV_PERSIST, eventfd_fn, NULL);
    event_add(&ev, NULL);
    pthread_create(&tid, NULL, thread_event, NULL);

    event_base_dispatch(base); 
    printf("================\n");
    sleep(10);
    return 0;
}
