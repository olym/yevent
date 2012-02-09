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
#include <stdio.h>
#include <assert.h>

#include "event.h"
#include "event-internal.h"
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

struct event_base *
event_base_new()
{
    struct event_base *base;

    if ((base = malloc(sizeof(struct event_base))) == NULL) {
        fprintf(stderr, "%s: calloc\n", __func__);
        return NULL;
    }

    TAILQ_INIT(&base->eventqueue);

    evmap_io_initmap(&base->iomap);

    base->evdata = NULL;

    base->evdata = epoll_init();
    if (base->evdata == NULL) {
        fprintf(stderr, "%s: no epoll_init error\n", __func__);
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
                if (ev->ev_events & EV_TIMEOUT)
                    read_timerfd(ev->ev_fd);
                (*ev->ev_callback)(
                        (int)ev->ev_fd, ev->ev_res, ev->ev_arg);
                break;
        }

		if (base->event_break)
			return -1;
    }
}

/*
 * Active events are stored in priority queues.  Lower priorities are always
 * process before higher priorities.  Low priority events can starve high
 * priority ones.
 */
static int
event_process_active(struct event_base *base)
{
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
    return c;
}

int
event_base_dispatch(struct event_base *base)
{
    int retval = 0;
    int res;

	if (base->running_loop) {
		fprintf(stderr, "%s: reentrant invocation.  Only one event_base_loop"
		    " can run on each event_base at once.", __func__);
		//EVBASE_RELEASE_LOCK(base, th_base_lock);
		return -1;
	}

	base->running_loop = 1;

    int done = 0;

    //base->th_owner_id = EVTHREAD_GET_ID();

    base->event_break = 0;

    while(!done) {
        if (base->event_break)
            break;

		/* If we have no events, we just exit */
		if (!event_haveevents(base) && !N_ACTIVE_CALLBACKS(base)) {
			fprintf(stderr, "%s: no events registered.", __func__);
			retval = 1;
			goto done;
		}

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

    if (ev->ev_events & EV_TIMEOUT && tv != NULL) {
        ev->ev_timeout = *tv;
        set_timerfd(ev->ev_fd, tv);
    }
    if (!(ev->ev_flags & (EVLIST_INSERTED|EVLIST_ACTIVE))) {
        res = evmap_io_add(base, ev->ev_fd, ev);
        if (res != -1)
            event_queue_insert(base, ev, EVLIST_INSERTED);
    }
}

int
event_add(struct event *ev, const struct timeval *tv)
{
	int res;

	if (!ev->ev_base) {
		fprintf(stderr, "%s: event has no event_base set.", __func__);
		return -1;
	}

	//EVBASE_ACQUIRE_LOCK(ev->ev_base, th_base_lock);

	res = event_add_internal(ev, tv, 0);

	//EVBASE_RELEASE_LOCK(ev->ev_base, th_base_lock);

	return (res);
}

/* Helper for event_del: always called with th_base_lock held. */
static inline int
event_del_internal(struct event *ev)
{
    struct event_base *base;
    int res = 0;

    if (ev->ev_base == NULL)
        return -1;
    base = ev->ev_base;

    if (ev->ev_flags & EVLIST_ACTIVE)
        event_queue_remove(base, ev, EVLIST_ACTIVE);
    if (ev->ev_flags & EVLIST_INSERTED) {
        event_queue_remove(base, ev, EVLIST_INSERTED);
        res = evmap_io_del(base, ev->ev_fd, ev);
    }

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

	//EVBASE_ACQUIRE_LOCK(ev->ev_base, th_base_lock);

	res = event_del_internal(ev);

	//EVBASE_RELEASE_LOCK(ev->ev_base, th_base_lock);

	return (res);
}

void
event_active(struct event *ev, int res)
{
	if (!ev->ev_base) {
		fprintf(stderr, "%s: event has no event_base set.", __func__);
		return;
	}

	//EVBASE_ACQUIRE_LOCK(ev->ev_base, th_base_lock);

	event_active_nolock(ev, res);

	//EVBASE_RELEASE_LOCK(ev->ev_base, th_base_lock);
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

    ev->ev_flags &= ~queue;
    switch (queue) {
    case EVLIST_INSERTED :
        TAILQ_REMOVE(&base->eventqueue, ev, ev_next);
        base->event_count--;
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

	ev->ev_flags |= queue;
	switch (queue) {
	case EVLIST_INSERTED:
		TAILQ_INSERT_TAIL(&base->eventqueue, ev, ev_next);
        base->event_count++;
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
