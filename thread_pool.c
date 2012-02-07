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

typedef void (*task_cb_fn)(void *);

struct thread_poll
{
    pthread_mutex_t lock;
    pthread_cond_t cond;
    char name[256];
    int num_threads;
    struct thread *threads;
    int running;
    //TODO
    //task double queue
};

struct thread_poll *
thread_poll_init(const char* name, int num_threads)
{
    struct thread_poll *poll;
    poll = malloc(sizeof(struct thread_poll));
    if (poll == NULL) {
        fprintf(stderr, "%s:malloc thread_poll error\n", __func__);
        return NULL;
    }

    snprintf(poll->name, sizeof(poll->name), "%s", name); 
    poll->num_threads = num_threads;
    poll->threads = malloc(num_threads * sizeof(struct thread));
    if (poll->threads == NULL) {
        fprintf(stderr, "%s:malloc threads error\n", __func__);
        return NULL;
    }
    poll->running = 0;
    //init mutex and cond
    //
    
    return poll;
}

void 
thread_poll_start(struct thread_poll* poll)
{
    struct thread *thd;
    char thread_name[256];
    poll->running = 1;
    for (i = 0; i < poll->num_threads; i++) {
        snprintf(thread_name, sizeof(thread_name), "%s%d", poll->name, i);
        thd = thread_init(_thread_poll_wait_run, thread_name, poll);
        poll->threads[i] = thd;
        thread_start(thd);
    }
}

void
thread_poll_stop(struct thread_poll *poll)
{

}

void
thread_poll_signal_run(struct thread_poll *poll, task_cb_fn task, void* args)
{
    if (poll->running == 0) {
        task(args);
    } else {
        evthread_posix_lock(poll->lock, 0);

        //TODO put task in thread_poll queue;
        //

        // signal the thread to run the task;
        evthread_cond_signal(poll->cond, 1); // signal the thread to run the task;

        evthread_posix_unlock(poll->lock, 0);
    }
}

static task_cb_fn
_thread_poll_take(struct thread_poll* poll)
{
    evthread_posix_lock(poll->lock, 0);
    while () {
        evthread_cond_wait(poll->cond, poll->lock, NULL);
    }
    if () {
        //TODO take task from queue
        //
    }
    evthread_posix_unlock(poll->lock, 0);
    return task;
}

static void
_thread_poll_wait_run(void* args)
{
    struct thread_poll *poll = args;
    while(poll->running) {

    }
}
