/*
 * =====================================================================================
 *
 *       Filename:  event.h
 *
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  01/28/2012 10:59:19 PM
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  YOUR NAME (), 
 *        Company:  
 *
 * =====================================================================================
 */

#ifndef _EVENT_H_
#define _EVENT_H_


#ifdef __cplusplus
extern "C" {
#endif
#include "event-internal.h"

struct event_base* event_init(void);
/**
  Initialize the event API.

  Use event_base_new() to initialize a new event base.

  @see event_base_set(), event_base_free(),
    event_base_new_with_config()
 */
struct event_base *event_base_new(void);

/**
  Threadsafe event dispatching loop.

  This loop will run the event base until either there are no more added
  events, or until something calls event_base_loopbreak() or
  evenet_base_loopexit().

  @param eb the event_base structure returned by event_init()
  @see event_init(), event_dispatch()
 */
int event_base_dispatch(struct event_base *);

int
event_base_loopbreak(struct event_base *event_base);
/**
  Deallocate all memory associated with an event_base, and free the base.

  Note that this function will not close any fds or free any memory passed
  to event_set as the argument to callback.

  @param eb an event_base to be freed
 */
void event_base_free(struct event_base *);

/**
  Associate a different event base with an event.

  @param eb the event base
  @param ev the event
 */
int event_base_set(struct event_base *, struct event *);

/**
  Handle events (threadsafe version).

  This is a more flexible version of event_base_dispatch().

  By default, this loop will run the event base until either there are no more
  added events, or until something calls event_base_loopbreak() or
  evenet_base_loopexit().  You can override this behavior with the 'flags'
  argument.

  @param eb the event_base structure returned by event_init()
  @param flags any combination of EVLOOP_ONCE | EVLOOP_NONBLOCK
  @return 0 if successful, -1 if an error occurred, or 1 if no events were
    registered.
  @see event_loopexit(), event_base_loop()
  */
int event_base_loop(struct event_base *, int);

typedef void (*event_callback_fn)(evutil_socket_t, short, void *);

/**
  Prepare an event structure to be added.

  The function event_assign() prepares the event structure ev to be used in
  future calls to event_add() and event_del().  The event will be prepared to
  call the function specified by the fn argument with an int argument
  indicating the file descriptor, a short argument indicating the type of
  event, and a void * argument given in the arg argument.  The fd indicates
  the file descriptor that should be monitored for events.  The events can be
  either EV_READ, EV_WRITE, or both.  Indicating that an application can read
  or write from the file descriptor respectively without blocking.

  The function fn will be called with the file descriptor that triggered the
  event and the type of event which will be either EV_TIMEOUT, EV_SIGNAL,
  EV_READ, or EV_WRITE.  The additional flag EV_PERSIST makes an event_add()
  persistent until event_del() has been called.

  Note that using event_assign() request that you have already allocated the
  event struct.  Doing so will often require your code to depend on the size
  of the structure, and will create possible incompatibility with future
  versions of Libevent.  If this seems like a bad idea to you, use event_new()
  and event_free() instead.

  @param ev an event struct to be modified
  @param base the event base to which ev should be attached.
  @param fd the file descriptor to be monitored
  @param event desired events to monitor; can be EV_READ and/or EV_WRITE
  @param fn callback function to be invoked when the event occurs
  @param arg an argument to be passed to the callback function

  @return 0 if success, or -1 on invalid arguments.

  @see event_add(), event_del(), event_once()

  */
int event_assign(struct event *, struct event_base *, evutil_socket_t, short, event_callback_fn, void *);

/**
  Create and allocate a new event structure, ready to be added.

  Arguments are as for event_assign; returns a newly allocated struct event *
  that must later be deallocated with event_free().

 */
struct event *event_new(struct event_base *, evutil_socket_t, short, event_callback_fn, void *);

/**
   Deallocate a struct event * returned by event_new().
 */
void event_free(struct event *);

/**
  Add an event to the set of monitored events.

  The function event_add() schedules the execution of the ev event when the
  event specified in event_set() occurs or in at least the time specified in
  the tv.  If tv is NULL, no timeout occurs and the function will only be
  called if a matching event occurs on the file descriptor.  The event in the
  ev argument must be already initialized by event_set() and may not be used
  in calls to event_set() until it has timed out or been removed with
  event_del().  If the event in the ev argument already has a scheduled
  timeout, the old timeout will be replaced by the new one.

  @param ev an event struct initialized via event_set()
  @param timeout the maximum amount of time to wait for the event, or NULL
         to wait forever
  @return 0 if successful, or -1 if an error occurred
  @see event_del(), event_set()
  */
int event_add(struct event *, const struct timeval *);

/**
  Remove an event from the set of monitored events.

  The function event_del() will cancel the event in the argument ev.  If the
  event has already executed or has never been added the call will have no
  effect.

  @param ev an event struct to be removed from the working set
  @return 0 if successful, or -1 if an error occurred
  @see event_add()
 */
int event_del(struct event *);


/**
  Make an event active.

  @param ev an event to make active.
  @param res a set of flags to pass to the event's callback.
  @param ncalls
 **/
void event_active(struct event *, int);

void event_active_nolock(struct event *ev, int res);

/**
   Get the socket assigned to an event.
*/
evutil_socket_t event_get_fd(const struct event *ev);

/**
   Get the event_base assigned to an event.
*/
struct event_base *event_get_base(const struct event *ev);

/**
   Return the events (EV_READ, EV_WRITE, etc) assigned to an event.
*/
short event_get_events(const struct event *ev);

/**
   Return the callback assigned to an event.
*/
event_callback_fn event_get_callback(const struct event *ev);

/**
   Return the callback argument assigned to an event.
*/
void *event_get_callback_arg(const struct event *ev);

/**
  Set the number of different event priorities (threadsafe variant).

  See the description of event_priority_init() for more information.

  @param eb the event_base structure returned by event_init()
  @param npriorities the maximum number of priorities
  @return 0 if successful, or -1 if an error occurred
  @see event_priority_init(), event_priority_set()
 */
int	event_base_priority_init(struct event_base *, int);


/**
  Assign a priority to an event.

  @param ev an event struct
  @param priority the new priority to be assigned
  @return 0 if successful, or -1 if an error occurred
  @see event_priority_init()
  */
int	event_priority_set(struct event *, int);

#ifdef __cplusplus
}
#endif

#endif /* _EVENT_H_ */
