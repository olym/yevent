/*
 * =====================================================================================
 *
 *       Filename:  test.c
 *
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  02/05/2012 11:19:35 AM
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  Yin Zhongxing (olym), zhongxing.yin@cs2c.com.cn
 *        Company:  www.CS2C.com.cn
 *
 * =====================================================================================
 */
#include <stdio.h>
#include <sys/time.h>
#include <signal.h>
#include <stdlib.h>
#include "event.h"
void timeout_test(int fd, short mask, void *args) {
    if (args != NULL)
        printf("%s\n", (char*)args);
    else
        printf("yevent test\n");
}
void signal_test(int fd, short mask, void *args) {
    if (args != NULL)
        printf("%s\n", (char *)args);
    exit(0);
}
int main(int argc, char *argv[])
{
    struct event_base *base;
    struct event *ev, *ev2;
    struct timeval tv, tv2;
    struct event ev3;

    tv.tv_sec = 2;
    tv.tv_usec = 0;
    tv2.tv_sec = 4;
    tv2.tv_usec = 0;

    base = event_init();

    ev = event_new(base, 0, EV_TIMEOUT | EV_PERSIST, timeout_test, NULL);
    ev2 = event_new(base, 0, EV_TIMEOUT, timeout_test, "yevent timeout test");

    event_assign(&ev3, base, SIGINT, EV_SIGNAL, signal_test, "yevent signal_test");

    event_add(ev, &tv);
    event_add(ev2, &tv2);
    event_add(&ev3, NULL);
    event_base_dispatch(base);
    return 0;
}
