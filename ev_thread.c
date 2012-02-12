/*
 * =====================================================================================
 *
 *       Filename:  thread_base.c
 *
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  02/12/2012 05:06:38 PM
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  Yin Zhongxing (olym), zhongxing.yin@cs2c.com.cn
 *        Company:  www.CS2C.com.cn
 *
 * =====================================================================================
 */

#include <stdio.h>
#include <stdlib.h>
#include "ev_thread.h"
#include "event.h"

static void *worker_event_thread(void *args);

struct event_base_thread *
event_base_thread_init()
{
    struct event_base_thread *base_thread;

    base_thread = malloc(sizeof(struct event_base_thread));
    if (base_thread == NULL){
        fprintf(stderr, "%s: Can't allocate thread\n", __func__);
        exit(1);
    }

    base_thread->lock = thread_posix_lock_alloc(0);
    if (!base_thread->lock) {
        fprintf(stderr, "%s: Can't allocate thread lock\n", __func__);
        exit(1);
    }

    base_thread->cond = thread_posix_cond_alloc();
    if (!base_thread->cond) {
        fprintf(stderr, "%s: Can't allocate thread cond\n", __func__);
        exit(1);
    }


    base_thread->thread = thread_init(NULL, worker_event_thread, base_thread);
    if (!base_thread->thread) {
        fprintf(stderr, "%s: Can't allocate thread\n", __func__);
        exit(1);
    }
    thread_start(base_thread->thread);

    base_thread->base = NULL; 
    /* Wait for the thread to set themselves up before returning. */
    thread_posix_lock(base_thread->lock, 0);
    while (base_thread->base == NULL) {
        thread_posix_cond_wait(base_thread->cond, base_thread->lock, NULL);
    }
    thread_posix_unlock(base_thread->lock, 0);

    return base_thread;
}

static void *worker_event_thread(void *args)
{
    struct event_base_thread *base_thread = args;

    thread_posix_lock(base_thread->lock, 0);
    base_thread->base = event_init();
    thread_posix_cond_signal(base_thread->cond, 1);
    thread_posix_unlock(base_thread->lock, 0);
    event_base_dispatch(base_thread->base);
    printf("%s:thread:event_base_dispach\n", __func__);
}
