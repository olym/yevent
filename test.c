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
#include "event.h"
void test(int fd, short mask, void *args) {
    printf("hello\n");
}
int main(int argc, char *argv[])
{
    struct event_base *base;
    struct event *ev;
    struct timeval tv;

    tv.tv_sec = 2;
    tv.tv_usec = 0;

    base = event_init();

    ev = event_new(base, 0, EV_TIMEOUT, test, NULL);

    event_add(ev, &tv);
    event_base_dispatch(base);
    return 0;
}
