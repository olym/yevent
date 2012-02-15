/*
 * =====================================================================================
 *
 *       Filename:  test_deferred.c
 *
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  02/12/2012 12:02:23 PM
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  Yin Zhongxing (olym), zhongxing.yin@cs2c.com.cn
 *        Company:  www.CS2C.com.cn
 *
 * =====================================================================================
 */
#include <stdio.h>
#include "event.h"
#include "ev_thread.h"
void deferred_fun(struct deferred_cb *cb, void *args)
{
    printf("deferred_cb_fn\n");
}
int main(int argc, char *argv[])
{
    struct event_base *base;
    struct deferred_cb cb;
    struct event_base_thread *base_thread;
    
    base = event_init();
    base_thread = event_base_thread_init();
    event_deferred_cb_init(&cb, deferred_fun, NULL);
    event_deferred_cb_schedule(&base_thread->base->defer_queue, &cb);
    event_base_dispatch(base);
    sleep(7);
    return 0;
}
