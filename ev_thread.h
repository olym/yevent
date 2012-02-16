/*
 * =====================================================================================
 *
 *       Filename:  ev_thread.h
 *
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  02/12/2012 07:15:06 PM
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  YOUR NAME (), 
 *        Company:  
 *
 * =====================================================================================
 */
#ifndef _EV_THREAD_H
#define _EV_THREAD_H
#include "thread.h"

struct event_base_thread
{
    struct event_base *base;
    struct thread *thread;
    pthread_mutex_t *lock;
    pthread_cond_t *cond;
    int started;
};
struct event_base_thread_pool
{
    struct event_base_thread **base_threads;
    struct event_base **bases;
    int num_base_thread;
    int cur_num;
}

struct event_base_thread * event_base_thread_init();
void event_base_thread_stop(struct event_base_thread *base_thread);

struct event_base_thread_pool *event_base_thread_pool_init(int num_thread);
struct event_base* get_next_base(struct event_base_thread_pool *);
void event_base_thread_pool_stop(struct event_base_thread_pool *)
#endif //_EV_THREAD_H
