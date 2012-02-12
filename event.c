/*
 * =====================================================================================
 *
 *       Filename:  event.c
 *
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  01/28/2012 09:59:01 PM
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  Yin Zhongxing (olym), zhongxing.yin@cs2c.com.cn
 *        Company:  www.CS2C.com.cn
 *
 * =====================================================================================
 */
#include <errno.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <sys/types.h>
#include <sys/eventfd.h>
#include <stdio.h>
#include <assert.h>

#include "event.h"
#include "event-internal.h"
#include "defer-internal.h"
#include "thread.h"
#include "epoll.h"
#include "eventfd.h"

/* Prototypes */
static inline int event_add_internal(struct event *ev,
    const struct timeval *tv, int tv_is_absolute);
static inline int event_del_internal(struct event *ev);
static int event_haveevents(struct event_base *base);

static void	event_queue_insert(struct event_base *, struct event *, int);
static void	event_queue_remove(struct event_base *, struct event *, int);
static int	event_process_active(struct event_base *);
static int event_process_deferred_active(struct deferred_cb_queue *, int *);
static void notify_base_cbq_callback(struct deferred_cb_queue *cb, void *baseptr);
static int evthread_notify_base(struct event_base *base);
static void evthread_notify_read(int fd, short what, void *args);

struct event_base *
event_base_new()
{
    struct event_base *base;
    int r;

    if ((base = malloc(sizeof(struct event_base))) == NULL) {
        fprintf(stderr, "%s: calloc\n", __func__);
        return NULL;
    }

    TAILQ_INIT(&base->eventqueue);

	event_deferred_cb_queue_init(&base->defer_queue);
	base->defer_queue.notify_fn = notify_base_cbq_callback;
	base->defer_queue.notify_arg = base;
    
    evmap_io_initmap(&base->iomap);

    base->evdata = NULL;
    base->evdata = epoll_init();

    if (base->evdata == NULL) {
        fprintf(stderr, "%s: no epoll_init error\n", __func__);
        event_base_free(base);
        return NULL;
    }

    //support thread
    base->th_base_lock = thread_posix_lock_alloc(0);
    base->defer_queue.lock = base->th_base_lock;
	/* prepare an event that we can use for wakeup */
    r = evthread_make_base_notifiable(base);
    if (r<0) {
        event_base_free(base);
        return NULL;
    }


	/* allocate a single active event queue */
	if (event_base_priority_init(base, 1) < 0) {
		event_base_free(base);
		return NULL;
	}
    
    return base;

}

void event_base_free(struct event_base *base)
{

}

struct event_base*
event_init(void)
{
    struct event_base *base = event_base_new();
    if (base == NULL) {
        fprintf(stderr, "%s: Unable to construct event_base\n", __func__);
        return NULL;
    }

    return base;
}

int
event_base_priority_init(struct event_base *base, int npriorities)
{
	int i;

	if (N_ACTIVE_CALLBACKS(base) || npriorities < 1 || npriorities > EVENT_MAX_PRIORITIES)
		return (-1);

	if (npriorities == base->nactivequeues)
		return (0);

	if (base->nactivequeues) {
		free(base->activequeues);
		base->nactivequeues = 0;
	}

	/* Allocate our priority queues */
	base->activequeues = (struct event_list *)
	  calloc(npriorities, sizeof(struct event_list));
	if (base->activequeues == NULL) {
		fprintf(stderr, "%s: calloc", __func__);
		return (-1);
	}
	base->nactivequeues = npriorities;

	for (i = 0; i < base->nactivequeues; ++i) {
		TAILQ_INIT(&base->activequeues[i]);
	}

	return (0);
}

/* Closure function invoked when we're activating a persistent event. */
static inline void
event_persist_closure(struct event_base *base, struct event *ev)
{
    thread_posix_unlock(base->th_base_lock, 0);
    if (ev->ev_events & EV_SIGNAL) {
        read_signalfd(ev->ev_fd);
    }
    if (ev->ev_events & EV_TIMEOUT) {
        read_timerfd(ev->ev_fd);
        set_timerfd(ev->ev_fd, &(ev->ev_timeout));
    }
	(*ev->ev_callback)((int)ev->ev_fd, ev->ev_res, ev->ev_arg);
}

/*
  Helper for event_process_active to process all the events in a single queue,
  releasing the lock as we go.  This function requires that the lock be held
  when it's invoked.  Returns -1 if we get a signal or an event_break that
  means we should stop processing any active events now.  Otherwise returns
  the number of non-internal events that we processed.
*/
static int
event_process_active_single_queue(struct event_base *base,
    struct event_list *activeq)
{
    struct event *ev;
    int count = 0;
    assert(activeq != NULL);

    for (ev = TAILQ_FIRST(activeq); ev; ev = TAILQ_FIRST(activeq)) {
        if (ev->ev_events & EV_PERSIST){
            event_queue_remove(base, ev, EVLIST_ACTIVE);
        }else
            event_del_internal(ev);
        ++count;

        switch(ev->ev_closure) {
            case EV_CLOSURE_PERSIST:
                event_persist_closure(base, ev);
                break;
            default:
            case EV_CLOSURE_NONE:
                thread_posix_unlock(base->th_base_lock, 0);
                if (ev->ev_events & EV_TIMEOUT)
                    read_timerfd(ev->ev_fd);
                (*ev->ev_callback)(
                        (int)ev->ev_fd, ev->ev_res, ev->ev_arg);
                break;
        }

        thread_posix_lock(base->th_base_lock, 0);
		if (base->event_break)
			return -1;
    }

    return count;
}

/*
 * Active events are stored in priority queues.  Lower priorities are always
 * process before higher priorities.  Low priority events can starve high
 * priority ones.
 */
static int
event_process_active(struct event_base *base)
{
    printf("%s\n", __func__);
    struct event_list *activeq = NULL;
    int i, c = 0;

    for (i = 0; i < base->nactivequeues; ++i) {
        if (TAILQ_FIRST(&base->activequeues[i]) != NULL) {
            activeq = &base->activequeues[i];
            c = event_process_active_single_queue(base, activeq);
            if (c < 0)
                return -1;
            else if (c > 0)
                break;
        }
    }
    //TODO
    event_process_deferred_active(&base->defer_queue, &base->event_break);
    return c;
}

static int
event_process_deferred_active(struct deferred_cb_queue *queue, int *breakptr)
{
	int count = 0;
	struct deferred_cb *cb;

#define MAX_DEFERRED 16
	while ((cb = TAILQ_FIRST(&queue->deferred_cb_list))) {
		cb->queued = 0;
		TAILQ_REMOVE(&queue->deferred_cb_list, cb, cb_next);
		--queue->active_count;
		UNLOCK_DEFERRED_QUEUE(queue);

		cb->cb(cb, cb->arg);

		LOCK_DEFERRED_QUEUE(queue);
		if (*breakptr)
			return -1;
		if (++count == MAX_DEFERRED)
			break;
	}
#undef MAX_DEFERRED
	return count;

}

int
event_base_dispatch(struct event_base *base)
{
    int retval = 0;
    int res;

	thread_posix_lock(base->th_base_lock, 0);

	if (base->running_loop) {
		fprintf(stderr, "%s: reentrant invocation.  Only one event_base_loop"
		    " can run on each event_base at once.", __func__);
		thread_posix_unlock(base->th_base_lock, 0);
		return -1;
	}

	base->running_loop = 1;

    int done = 0;

    base->th_owner_id = EVTHREAD_GET_ID();

    base->event_break = 0;

    while(!done) {
        if (base->event_break)
            break;

		/* If we have no events, we just exit */
		//if (!event_haveevents(base) && !N_ACTIVE_CALLBACKS(base)) {
		//	fprintf(stderr, "%s: no events registered\n", __func__);
		//	retval = 1;
		//	goto done;
		//}

        res = epoll_dispatch(base, NULL);

		if (res == -1) {
			fprintf(stderr, "%s: dispatch returned unsuccessfully.", __func__);
			retval = -1;
			goto done;
		}

		if (N_ACTIVE_CALLBACKS(base)) {
			int n = event_process_active(base);
        } else {
            done = 1;
        }
    }
done:
    base->running_loop = 0;

    thread_posix_unlock(base->th_base_lock, 0);

    return retval;
}

/* Returns true iff we're currently watching any events. */
static int
event_haveevents(struct event_base *base)
{
	/* Caller must hold th_base_lock */
	return (base->event_count > 0);
}

int
event_assign(struct event *ev, struct event_base *base, evutil_socket_t fd, short events, void (*callback)(evutil_socket_t, short, void *), void *arg)
{
    ev->ev_base = base;
    ev->ev_callback = callback;
    ev->ev_arg = arg;
    ev->ev_fd = fd;
    ev->ev_events = events;
    ev->ev_res = 0;
    ev->ev_flags = EVLIST_INIT;

    if (events & EV_SIGNAL) {
        ev->ev_fd = signalfd_create(fd);
        ev->ev_events |= EV_READ | EV_PERSIST;
    } else if (events & EV_TIMEOUT) {
        ev->ev_fd = create_timerfd();
        ev->ev_events |= EV_READ;
    }
    if (ev->ev_events & EV_PERSIST) {
        ev->ev_closure = EV_CLOSURE_PERSIST;
    } else {
        ev->ev_closure = EV_CLOSURE_NONE;
    }

	if (base != NULL) {
		/* by default, we put new events into the middle priority */
		ev->ev_pri = base->nactivequeues / 2;
	}

    return 0;
}

struct event *
event_new(struct event_base *base, evutil_socket_t fd, short events, void (*cb)(evutil_socket_t, short, void *), void *arg)
{
	struct event *ev;
	ev = malloc(sizeof(struct event));
	if (ev == NULL)
		return (NULL);
	if (event_assign(ev, base, fd, events, cb, arg) < 0) {
		free(ev);
		return (NULL);
	}

	return (ev);
}

void
event_free(struct event *ev)
{
	/* make sure that this event won't be coming back to haunt us. */
	event_del(ev);
	free(ev);
}

/*
 * Set's the priority of an event - if an event is already scheduled
 * changing the priority is going to fail.
 */

int
event_priority_set(struct event *ev, int pri)
{
	if (ev->ev_flags & EVLIST_ACTIVE)
		return (-1);
	if (pri < 0 || pri >= ev->ev_base->nactivequeues)
		return (-1);

	ev->ev_pri = pri;

	return (0);
}
/* Implementation function to add an event.  Works just like event_add,
 * except: 1) it requires that we have the lock.  2) if tv_is_absolute is set,
 * we treat tv as an absolute time, not as an interval to add to the current
 * time */
static inline int
event_add_internal(struct event *ev, const struct timeval *tv,
    int tv_is_absolute)
{
    struct event_base *base = ev->ev_base;
    int res = 0;
    int wakeup = 0;

    if (ev->ev_events & EV_TIMEOUT && tv != NULL) {
        ev->ev_timeout = *tv;
        set_timerfd(ev->ev_fd, tv);
    }
    if (!(ev->ev_flags & (EVLIST_INSERTED|EVLIST_ACTIVE))) {
        res = evmap_io_add(base, ev->ev_fd, ev);
        if (res != -1)
            event_queue_insert(base, ev, EVLIST_INSERTED);
        if (res == 1) {
            wakeup = 1;
            res = 0;
        }
    }

    if (res != -1 && wakeup && EVBASE_NEED_NOTIFY(base))
        evthread_notify_base(base);

    return res;
}

int
event_add(struct event *ev, const struct timeval *tv)
{
	int res;
	if (!ev->ev_base) {
		fprintf(stderr, "%s: event has no event_base set.", __func__);
		return -1;
	}

    struct event_base *base = ev->ev_base;

	thread_posix_lock(base->th_base_lock, 0);

	res = event_add_internal(ev, tv, 0);

	thread_posix_unlock(base->th_base_lock, 0);

	return (res);
}

/* Helper for event_del: always called with th_base_lock held. */
static inline int
event_del_internal(struct event *ev)
{
    struct event_base *base;
    int res = 0;
    int wakeup = 0;

    if (ev->ev_base == NULL)
        return -1;
    base = ev->ev_base;

    //if (ev->ev_flags & EVLIST_TIMEOUT) {
    //    event_queue_remove(base, ev, EVLIST_TIMEOUT);
    //}
    if (ev->ev_flags & EVLIST_ACTIVE)
        event_queue_remove(base, ev, EVLIST_ACTIVE);

    if (ev->ev_flags & EVLIST_INSERTED) {
        event_queue_remove(base, ev, EVLIST_INSERTED);
        res = evmap_io_del(base, ev->ev_fd, ev);
        if (res == 1) {
            wakeup = 1;
            res = 0;
        }
    }

    if (res != -1 && wakeup && EVBASE_NEED_NOTIFY(base))
        evthread_notify_base(base);

    return res;
}

int
event_del(struct event *ev)
{
	int res;

	if (!ev->ev_base) {
		fprintf(stderr, "%s: event has no event_base set.", __func__);
		return -1;
	}
    struct event_base *base = ev->ev_base;

	thread_posix_lock(base->th_base_lock, 0);

	res = event_del_internal(ev);

	thread_posix_unlock(base->th_base_lock, 0);

	return (res);
}

void
event_deferred_cb_queue_init(struct deferred_cb_queue *cb)
{
	memset(cb, 0, sizeof(struct deferred_cb_queue));
	TAILQ_INIT(&cb->deferred_cb_list);
}

/** Helper for the deferred_cb queue: wake up the event base. */
static void
notify_base_cbq_callback(struct deferred_cb_queue *cb, void *baseptr)
{
	struct event_base *base = baseptr;
    printf("base->th_own_id = %d tid = %d runting = %d\n", base->th_owner_id, tid(), base->running_loop);
	if (EVBASE_NEED_NOTIFY(base)) {
        printf("%s\n", __func__);
		evthread_notify_base(base);
    }
}

void
event_deferred_cb_init(struct deferred_cb *cb, deferred_cb_fn fn, void *arg)
{
	memset(cb, 0, sizeof(struct deferred_cb));
	cb->cb = fn;
	cb->arg = arg;
}

void
event_deferred_cb_cancel(struct deferred_cb_queue *queue,
    struct deferred_cb *cb)
{
	if (!queue) {
        return;
	}

	LOCK_DEFERRED_QUEUE(queue);
	if (cb->queued) {
		TAILQ_REMOVE(&queue->deferred_cb_list, cb, cb_next);
		--queue->active_count;
		cb->queued = 0;
	}
	UNLOCK_DEFERRED_QUEUE(queue);
}

void
event_deferred_cb_schedule(struct deferred_cb_queue *queue,
    struct deferred_cb *cb)
{
	if (!queue) {
        return;
	}

	LOCK_DEFERRED_QUEUE(queue);
	if (!cb->queued) {
		cb->queued = 1;
		TAILQ_INSERT_TAIL(&queue->deferred_cb_list, cb, cb_next);
		++queue->active_count;
		if (queue->notify_fn) {
			queue->notify_fn(queue, queue->notify_arg);
        }
	}
	UNLOCK_DEFERRED_QUEUE(queue);
}
void
event_active(struct event *ev, int res)
{
	if (!ev->ev_base) {
		fprintf(stderr, "%s: event has no event_base set.", __func__);
		return;
	}
    struct event_base *base = ev->ev_base;

	thread_posix_lock(base->th_base_lock, 0);

	event_active_nolock(ev, res);

	thread_posix_unlock(base->th_base_lock, 0);
}

void
event_active_nolock(struct event *ev, int res)
{
    struct event_base *base;
    
    base = ev->ev_base;
    
    ev->ev_res = res;  //result passed to event callback
    event_queue_insert(base, ev, EVLIST_ACTIVE);
}

/* Remove 'ev' from 'queue' (EVLIST_...) in base. */
static void
event_queue_remove(struct event_base *base, struct event *ev, int queue)
{
    if (!(ev->ev_flags & queue)) {
        fprintf(stderr, "%s: %p(fd %d) not on queue %x", __func__,
                ev, ev->ev_fd, queue);
        return;
    }

	if (~ev->ev_flags & EVLIST_INTERNAL)
		base->event_count--;

    ev->ev_flags &= ~queue;
    switch (queue) {
    case EVLIST_INSERTED :
        TAILQ_REMOVE(&base->eventqueue, ev, ev_next);
        break;
    case EVLIST_ACTIVE :
        base->event_count_active--;
        TAILQ_REMOVE(&base->activequeues[ev->ev_pri], ev, ev_active_next);
        break;
    default:
        fprintf(stderr, "%s: unknown queue %x", __func__, queue);
    }
}

static void
event_queue_insert(struct event_base *base, struct event *ev, int queue)
{

	if (ev->ev_flags & queue) {
		/* Double insertion is possible for active events */
		if (queue & EVLIST_ACTIVE)
			return;

		fprintf(stderr, "%s: %p(fd %d) already on queue %x", __func__,
			   ev, ev->ev_fd, queue);
		return;
	}
    if (~ev->ev_flags & EVLIST_INTERNAL)
        base->event_count++;
	ev->ev_flags |= queue;
	switch (queue) {
	case EVLIST_INSERTED:
		TAILQ_INSERT_TAIL(&base->eventqueue, ev, ev_next);
		break;
	case EVLIST_ACTIVE:
		base->event_count_active++;
		TAILQ_INSERT_TAIL(&base->activequeues[ev->ev_pri],
		    ev,ev_active_next);
		break;
	default:
		fprintf(stderr, "%s: unknown queue %x", __func__, queue);
	}
}
static void
evthread_notify_read(int fd, short what, void *args)
{
    struct event_base *base = args;
    uint64_t one;
    int n = read(base->notifyfd, &one, sizeof(one));
    if (n != sizeof one) {
        fprintf(stderr, "%s: read error\n", __func__);
    }
    thread_posix_lock(base->th_base_lock, 0);
    base->is_notify_pending = 0;
    thread_posix_unlock(base->th_base_lock, 0);
}

/** Tell the thread currently running the event_loop for base (if any) that it
 * needs to stop waiting in its dispatch function (if it is) and process all
 * active events and deferred callbacks (if there are any).  */
static int
evthread_notify_base(struct event_base *base)
{
    printf("%s: tid = %ld\n", __func__, base->th_owner_id);
  uint64_t one = 1;
  //if the base already has a pending notify, we don't need to write any more
  if (!base->is_notify_pending) {
      base->is_notify_pending = 1;
      ssize_t n = write(base->notifyfd, &one, sizeof one);
      if (n != sizeof one) {
          fprintf(stderr, "%s:write %d bytes instead of 8", __func__, n);
      }
  }
}
int
evthread_make_base_notifiable(struct event_base *base)
{
    if (!base)
        return -1;
    base->notifyfd = eventfd(0, EFD_NONBLOCK | EFD_CLOEXEC);
    if (base->notifyfd < 0) {
        fprintf(stderr, "%s:Failed to create event fd\n", __func__);
        return -1;
    }
	event_assign(&base->notify_ev, base, base->notifyfd,
				 EV_READ|EV_PERSIST, evthread_notify_read, base);

	/* we need to mark this as internal event */
	base->notify_ev.ev_flags |= EVLIST_INTERNAL;
	event_priority_set(&base->notify_ev, 0);

	return event_add(&base->notify_ev, NULL);
}

