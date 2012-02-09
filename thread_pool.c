/*
 * =====================================================================================
 *
 *       Filename:  thread_pool.c
 *
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  02/07/2012 11:53:05 PM
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  Yin Zhongxing (olym), zhongxing.yin@cs2c.com.cn
 *        Company:  www.CS2C.com.cn
 *
 * =====================================================================================
 */
#include <stdio.h>
#include <time.h>
#include <stdlib.h>
#include <string.h>
#include "thread_pool.h"
#include "thread.h"

static struct task* _thread_pool_take(struct thread_pool* pool);
static void* _thread_pool_wait_run(void* args);
    
struct thread_pool *
thread_pool_init(const char* name, int num_threads)
{
    struct thread_pool *pool;

    pool = malloc(sizeof(struct thread_pool));
    if (pool == NULL) {
        fprintf(stderr, "%s:malloc thread_pool error\n", __func__);
        return NULL;
    }

    snprintf(pool->name, sizeof(pool->name), "%s", name); 
    pool->num_threads = num_threads;
    pool->threads = malloc(num_threads * sizeof(struct thread *));
    if (pool->threads == NULL) {
        fprintf(stderr, "%s:malloc threads error\n", __func__);
        return NULL;
    }
    pool->running = 0;
    TAILQ_INIT(&pool->taskqueue);
    //init mutex and cond
    pool->lock = evthread_posix_lock_alloc(0);
    pool->cond = evthread_posix_cond_alloc();
    
    return pool;
}
void
thread_pool_stop(struct thread_pool* pool)
{
    int i;

    pool->running = 0;
    evthread_posix_cond_signal(pool->cond, 1);
    for (i = 0; i < pool->num_threads; i++) {
        thread_join(pool->threads[i]);
    }
    free(pool->threads);
    pool->threads = NULL;
    evthread_posix_lock_free(pool->lock, 0); 
    evthread_posix_cond_free(pool->cond);
}
int
thread_pool_start(struct thread_pool* pool)
{
    int i;

    struct thread *thd;
    char thread_name[256];
    pool->running = 1;
    for (i = 0; i < pool->num_threads; i++) {
        snprintf(thread_name, sizeof(thread_name), "%s%d", pool->name, i);
        thd = thread_init(thread_name, _thread_pool_wait_run, pool);
        if (thd == NULL) {
            return -1;
        }
        pool->threads[i] = thd;
        thread_start(thd);
    }
    return 0;
}

int
thread_pool_signal_run(struct thread_pool *pool, task_cb_fn task_cb, void* args)
{
    if (pool->running == 0) {
        task_cb(args);
    } else {
        evthread_posix_lock(pool->lock, 0);

        // put task in thread_pool queue;
        struct task *task = malloc(sizeof(struct task));
        if (task == NULL) {
            fprintf(stderr, "%s: malloc error\n", __func__);
            return -1;
        } 
        task->task_cb = task_cb;
        task->args = strdup(args);
        TAILQ_INSERT_TAIL(&pool->taskqueue, task, task_next);

        // signal the thread to run the task;
        evthread_posix_cond_signal(pool->cond, 1); // signal the thread to run the task;

        evthread_posix_unlock(pool->lock, 0);
    }
    return 0;
}

static struct task*
_thread_pool_take(struct thread_pool* pool)
{
    evthread_posix_lock(pool->lock, 0);
    while (TAILQ_EMPTY(&pool->taskqueue)) {
        evthread_posix_cond_wait(pool->cond, pool->lock, NULL);
    }
    struct task *task;
    if (!TAILQ_EMPTY(&pool->taskqueue)) {
        // take task from queue
        task = TAILQ_FIRST(&pool->taskqueue);
        TAILQ_REMOVE(&pool->taskqueue, task, task_next);
    }
    evthread_posix_unlock(pool->lock, 0);
    return task;
}

static void *
_thread_pool_wait_run(void* args)
{
    struct thread_pool *pool = args;
    while(pool->running) {
        struct task *task = _thread_pool_take(pool);
        if (task)
            task->task_cb(task->args);
    }
    return NULL;
}
