/*
 * =====================================================================================
 *
 *       Filename:  event-internal.h
 *
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  01/28/2012 10:34:52 PM
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  YOUR NAME (), 
 *        Company:  
 *
 * =====================================================================================
 */

#ifndef _EVENT_INTERNAL_H_
#define _EVENT_INTERNAL_H_

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>
#include <sys/queue.h>
#include <sys/time.h>
#include "defer-internal.h"
#define evutil_socket_t int

#define EVENT_MAX_PRIORITIES 256

struct event {
	TAILQ_ENTRY(event) ev_active_next;
	TAILQ_ENTRY(event) ev_next;
	
	evutil_socket_t ev_fd;

	struct event_base *ev_base;

    /* used for io events */
    struct {
        TAILQ_ENTRY(event) ev_io_next;
        struct timeval ev_timeout;
    } ev_io;

	short ev_events;
	short ev_res;		/* result passed to event callback */
	short ev_flags;
	uint8_t ev_pri;	/* smaller numbers are higher priority */
	uint8_t ev_closure;
	struct timeval ev_timeout;

	/* allows us to adopt for different types of events */
	void (*ev_callback)(evutil_socket_t, short, void *arg);
	void *ev_arg;
};

TAILQ_HEAD (event_list, event);

/* Flags to pass to event_set(), event_new(), event_assign(),
 * event_pending(), and anything else with an argument of the form
 * "short events" */
#define EV_TIMEOUT	0x01
#define EV_READ		0x02
#define EV_WRITE	0x04
#define EV_SIGNAL	0x08
/** Persistent event: won't get removed automatically when activated. */
#define EV_PERSIST	0x10
/** Select edge-triggered behavior, if supported by the backend. */
#define EV_ET       0x20

#define EVLIST_TIMEOUT  0x01
#define EVLIST_INSERTED	0x02
#define EVLIST_SIGNAL 0x04
#define EVLIST_ACTIVE   0x08
#define EVLIST_INTERNAL 0x10
#define EVLIST_INIT	0x80

/* Possible values for ev_closure in struct event. */
#define EV_CLOSURE_NONE 0
#define EV_CLOSURE_PERSIST 1

struct event_io_map {
    /* An array of evmap_io; empty entries are set to NULL.
     */
    void **entries;
    /* The number of entries available in entries */
    int nentries;
};

struct event_base {
    void *evdata;

	/** Number of total events added to this event_base */
	int event_count;
	/** Number of total events active in this event_base */
	int event_count_active;

	/** Set if we should terminate the loop immediately */
	int event_break;

	/** Set if we're running the event_base_loop function, to prevent
	 * reentrant invocation. */
	int running_loop;

	/** True if the base already has a pending notify, and we don't need
	 * to add any more. */
    int is_notify_pending;
    int notifyfd;
    struct event notify_ev;
	/* Active event management. */
	/** An array of nactivequeues queues for active events (ones that
	 * have triggered, and whose callbacks need to be called).  Low
	 * priority numbers are more important, and stall higher ones.
	 */
	struct event_list *activequeues;
	/** The length of the activequeues array */
	int nactivequeues;

    struct deferred_cb_queue defer_queue;

	/** Mapping from file descriptors to enabled (added) events */
	struct event_io_map iomap;
	/** All events that have been enabled (added) in this event_base */
	struct event_list eventqueue;

	/* threading support */
	/** The thread currently running the event_loop for this base */
	unsigned long th_owner_id;
	/** A lock to prevent conflicting accesses to this event_base */
	void *th_base_lock;
	/** The event whose callback is executing right now */
	struct event *current_event;
};

#define N_ACTIVE_CALLBACKS(base)					\
	((base)->event_count_active)

#ifdef __cplusplus
}
#endif

#endif
