/*
 * =====================================================================================
 *
 *       Filename:  thread.h
 *
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  02/09/2012 11:06:29 AM
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  YOUR NAME (), 
 *        Company:  
 *
 * =====================================================================================
 */
#ifndef _THREAD_H
#define _THREAD_H
#include <pthread.h>

#define EVTHREAD_LOCKTYPE_RECURSIVE 1

/** A flag passed to a locking callback when the lock was allocated as a
 * read-write lock, and we want to acquire or release the lock for writing. */
#define EVTHREAD_WRITE	0x04
/** A flag passed to a locking callback when the lock was allocated as a
 * read-write lock, and we want to acquire or release the lock for reading. */
#define EVTHREAD_READ	0x08
/** A flag passed to a locking callback when we don't want to block waiting
 * for the lock; if we can't get the lock immediately, we will instead
 * return nonzero from the locking callback. */
#define EVTHREAD_TRY    0x10

typedef void* (*thread_cb_fn)(void *);

struct thread {
    char *name;
    pid_t tid;
    pthread_t pthreadid;
    int started;
    thread_cb_fn thread_cb;
    void *args;
};

struct thread *thread_init(const char* name, thread_cb_fn thread_cb, void *args);
void thread_start(struct thread* thd);
void thread_join(struct thread* thd);

void * evthread_posix_lock_alloc(unsigned locktype);
void evthread_posix_lock_free(void *_lock, unsigned locktype);
void evthread_posix_lock(void *_lock, unsigned mode);
void evthread_posix_unlock(void *_lock, unsigned mode);
unsigned long evthread_posix_get_id(void);
void* evthread_posix_cond_alloc();
void evthread_posix_cond_free(void *_cond);
int evthread_posix_cond_wait(void *_cond, void *_lock, const struct timeval *tv);
int evthread_posix_cond_signal(void *_cond, int broadcast);

#endif
