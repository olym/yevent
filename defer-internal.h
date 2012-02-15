/*
 * =====================================================================================
 *
 *       Filename:  defer-internal.h
 *
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  02/11/2012 07:26:45 PM
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  YOUR NAME (), 
 *        Company:  
 *
 * =====================================================================================
 */
#ifndef _DEFER_INTERNAL_H
#define _DEFER_INTERNAL_H

struct deferred_cb;
typedef void (*deferred_cb_fn)(struct deferred_cb *, void *);
/** A deferred_cb is a callback that can be scheduled to run as part of
 * an event_base's event_loop, rather than running immediately. */
struct deferred_cb {
	/** Links to the adjacent active (pending) deferred_cb objects. */
	TAILQ_ENTRY (deferred_cb) cb_next;
	/** True iff this deferred_cb is pending in an event_base. */
	unsigned queued : 1;
	/** The function to execute when the callback runs. */
	deferred_cb_fn cb;
	/** The function's second argument. */
	void *arg;
};

/** A deferred_cb_queue is a list of deferred_cb that we can add to and run. */
struct deferred_cb_queue {
	/** Lock used to protect the queue. */
	void *lock;

	/** How many entries are in the queue? */
	int active_count;

	/** Function called when adding to the queue from another thread. */
	void (*notify_fn)(struct deferred_cb_queue *, void *);
	void *notify_arg;

	/** Deferred callback management: a list of deferred callbacks to
	 * run active the active events. */
	TAILQ_HEAD (deferred_cb_list, deferred_cb) deferred_cb_list;
};

/**
   Initialize an empty, non-pending deferred_cb.

   @param deferred The deferred_cb structure to initialize.
   @param cb The function to run when the deferred_cb executes.
   @param arg The function's second argument.
 */
void event_deferred_cb_init(struct deferred_cb *, deferred_cb_fn, void *);
/**
   Cancel a deferred_cb if it is currently scheduled in an event_base.
 */
void event_deferred_cb_cancel(struct deferred_cb_queue *, struct deferred_cb *);
/**
   Activate a deferred_cb if it is not currently scheduled in an event_base.
 */
void event_deferred_cb_schedule(struct deferred_cb_queue *, struct deferred_cb *);

struct deferred_cb_queue *
event_get_deferred_cb_queue(struct event_base *base);

#define LOCK_DEFERRED_QUEUE(q)						\
    thread_posix_lock((q)->lock, 0)
#define UNLOCK_DEFERRED_QUEUE(q)					\
    thread_posix_unlock((q)->lock, 0)

void event_deferred_cb_queue_init(struct deferred_cb_queue *cb);
#endif
