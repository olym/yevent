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

#define ev_io_next	_ev.ev_io.ev_io_next

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

	/* Active event management. */
	/** An array of nactivequeues queues for active events (ones that
	 * have triggered, and whose callbacks need to be called).  Low
	 * priority numbers are more important, and stall higher ones.
	 */
	struct event_list *activequeues;
	/** The length of the activequeues array */
	int nactivequeues;

	/** Mapping from file descriptors to enabled (added) events */
	struct event_io_map iomap;
	/** All events that have been enabled (added) in this event_base */
	struct event_list eventqueue;
};

#define N_ACTIVE_CALLBACKS(base)					\
	((base)->event_count_active + (base)->defer_queue.active_count)
