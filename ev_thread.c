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

    base_thread->started = 0;
    base_thread->lock = thread_posix_lock_alloc(0);
    if (!base_thread->lock) {
        fprintf(stderr, "%s: Can't allocate thread lock\n", __func__);
        return NULL;
    }

    base_thread->cond = thread_posix_cond_alloc();
    if (!base_thread->cond) {
        fprintf(stderr, "%s: Can't allocate thread cond\n", __func__);
        return NULL;
    }


    base_thread->thread = thread_init(NULL, worker_event_thread, base_thread);
    if (!base_thread->thread) {
        fprintf(stderr, "%s: Can't allocate thread\n", __func__);
        return NULL;
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
    struct event ev;
    thread_posix_lock(base_thread->lock, 0);
    base_thread->started = 1;
    base_thread->base = event_init();
    thread_posix_cond_signal(base_thread->cond, 1);
    thread_posix_unlock(base_thread->lock, 0);
    event_base_dispatch(base_thread->base);
}

void
event_base_thread_stop(struct event_base_thread *base_thread)
{
    struct event_base *base_loop = base_thread->base;
    event_base_loopbreak(base_loop);
    thread_join(base_thread->thread);
    thread_posix_lock_free(base_thread->lock);
    thread_posix_cond_free(base_thread->cond);
    event_base_free(base_loop);
}


struct event_base_thread_pool *event_base_thread_pool_init(int num_thread)
{
    int i;
    struct event_base_thread_pool *pool;

    pool = malloc(sizeof(struct event_base_thread_pool));  
    if (pool == NULL) {
        fprintf(stderr, "%s: malloc error\n", __func__);
        return NULL;
    }
    pool->base_threads = malloc(num_thread * sizeof(struct event_base_thread *));
    if (pool->base_threads == NULL) {
        fprintf(stderr, "%s: malloc error\n", __func__);
        return NULL;
    }

    pool->bases = malloc(num_thread * sizeof(struct event_base *));
    if (pool->base_threads == NULL) {
        fprintf(stderr, "%s: malloc error\n", __func__);
        return NULL;
    }

    for (i = 0; i < num_thread; i++) {
        pool->base_threads[i] = event_base_thread_init();
        if (pool->base_threads[i] == NULL)
           return NULL; 
        pool->bases[i] = pool->base_threads[i]->base;
    }
    pool->cur_num = 0;
    pool->num_thread = num_thread;

    return pool;
}

struct event_base* get_next_base(struct event_base_thread_pool *pool)
{
    struct event_base *base;

    base = pool->bases[pool->cur_num++];
    if (pool->cur_num > pool->num_thread)
        pool->cur_num = 0;

    return base;
}
void event_base_thread_pool_stop(struct event_base_thread_pool *pool)
{
    int i;

    for (i = 0; i < pool->num_thread; i++) {
        event_base_thread_stop(pool->base_threads[i]);
    }

    free(pool->bases);
    free(pool->base_threads);
    free(pool);
}
