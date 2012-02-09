/*
 * =====================================================================================
 *
 *       Filename:  thread.c
 *
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  02/07/2012 09:17:16 PM
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
#include <sys/time.h>
#include <time.h>
#include <errno.h>
#include <string.h>
#include <sys/syscall.h>
#include <assert.h>

#include "thread.h"

static pthread_mutexattr_t attr_recursive;

pid_t tid()
{
    pid_t tid;
    tid = syscall(SYS_gettid);
    return tid;
}
struct thread *thread_init(const char* name, thread_cb_fn thread_cb, void *args)
{
    struct thread *thd;
    thd = malloc(sizeof(struct thread));
    if (thd == NULL) {
        fprintf(stderr, "%s:malloc error\n", __func__);
        return NULL;
    }
    thd->name = strdup(name);
    thd->thread_cb = thread_cb;
    thd->args = args;
    thd->tid = 0;
    thd->pthreadid = 0;
    return thd;
}
void thread_start(struct thread* thd)
{
    thd->started = 1;
    pthread_create(&thd->pthreadid, NULL, thd->thread_cb, thd->args);
}
void thread_join(struct thread* thd)
{
    assert(thd->started);
    pthread_join(thd->pthreadid, NULL);
}

void *
evthread_posix_lock_alloc(unsigned locktype)
{
	pthread_mutexattr_t *attr = NULL;
	pthread_mutex_t *lock = malloc(sizeof(pthread_mutex_t));
	if (!lock)
		return NULL;
	if (locktype & EVTHREAD_LOCKTYPE_RECURSIVE)
		attr = &attr_recursive;
	if (pthread_mutex_init(lock, attr)) {
		free(lock);
		return NULL;
	}
	return lock;
}

void
evthread_posix_lock_free(void *_lock, unsigned locktype)
{
    pthread_mutex_t *lock = _lock;
    pthread_mutex_destroy(lock);
    free(lock);
}

void
evthread_posix_lock(void *_lock, unsigned mode)
{
    pthread_mutex_t *lock = _lock;
    if (mode & EVTHREAD_TRY) {
        pthread_mutex_trylock(lock);
    } else {
        pthread_mutex_lock(lock);
    }
}

void
evthread_posix_unlock(void *_lock, unsigned mode)
{
    pthread_mutex_t *lock = _lock;
    pthread_mutex_unlock(lock);
}

unsigned long
evthread_posix_get_id(void)
{
    pthread_t thid;
    thid = pthread_self();
    return (unsigned long)thid;
}

void*
evthread_posix_cond_alloc()
{
    pthread_cond_t *cond = malloc(sizeof(pthread_cond_t));
    if (!cond)
        return NULL;
    if (pthread_cond_init(cond, NULL)) {
        free(cond);
        return NULL;
    }

    return cond;
}

void
evthread_posix_cond_free(void *_cond)
{
    pthread_cond_t *cond = _cond;
    pthread_cond_destroy(cond);
    free(cond);
}

int 
evthread_posix_cond_wait(void *_cond, void *_lock, const struct timeval *tv)
{
    pthread_cond_t *cond = _cond;
    pthread_mutex_t *lock = _lock;
    int r;

    if (tv) {
        struct timeval now, abstime;
        struct timespec ts;
        gettimeofday(&now, NULL);
        timeradd(&now, tv, &abstime);
        ts.tv_sec = abstime.tv_sec;
        ts.tv_nsec = abstime.tv_usec * 1000;
        r = pthread_cond_timedwait(cond, lock, &ts);
        if (r = ETIMEDOUT)
            return 1;
        else if (r)
            return -1;
        else 
            return 0;
    } else {
        r = pthread_cond_wait(cond, lock);
        return r ? -1 : 0;
    }
}

int
evthread_posix_cond_signal(void *_cond, int broadcast)
{
    pthread_cond_t *cond = _cond;
    int r;    
    if(broadcast) 
        r = pthread_cond_broadcast(cond);
    else
        r = pthread_cond_signal(cond);

    return r?-1:0;
}
