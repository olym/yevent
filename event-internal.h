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
	struct event_io_map io;
	/** All events that have been enabled (added) in this event_base */
	struct event_list eventqueue;
};

#define N_ACTIVE_CALLBACKS(base)					\
	((base)->event_count_active + (base)->defer_queue.active_count)
