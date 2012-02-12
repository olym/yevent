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
};
struct event_base_thread * event_base_thread_init();
#endif //_EV_THREAD_H
