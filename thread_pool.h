/*
 * =====================================================================================
 *
 *       Filename:  thread_pool.h
 *
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  02/09/2012 10:57:25 AM
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  YOUR NAME (), 
 *        Company:  
 *
 * =====================================================================================
 */
#ifndef _THREAD_POOL_H
#define _THREAD_POLL_H

#include <pthread.h>
#include <sys/queue.h>

#include "thread.h"

typedef void* (*task_cb_fn)(void *);

struct task {
    TAILQ_ENTRY(task) task_next;
    task_cb_fn task_cb;
    void *args;
};

TAILQ_HEAD(task_list, task);

struct thread_pool
{
    pthread_mutex_t *lock;
    pthread_cond_t *cond;
    char name[256];
    int num_threads;
    struct thread **threads;
    int running;
    struct task_list taskqueue;
};


struct thread_pool *thread_pool_init(const char* name, int num_threads);
void thread_pool_stop(struct thread_pool* pool);
int thread_pool_start(struct thread_pool* pool);
int thread_pool_run(struct thread_pool *pool, task_cb_fn task_cb, void* args);

#endif 
