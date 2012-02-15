/*
 * =====================================================================================
 *
 *       Filename:  event_struct.h
 *
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  01/31/2012 11:39:41 AM
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  YOUR NAME (), 
 *        Company:  
 *
 * =====================================================================================
 */

#ifndef _EVENT_STRUCT_H_
#define _EVENT_STRUCT_H_

#define EVLIST_INSERTED	0x01
#define EVLIST_ACTIVE	0x02


#ifdef __cplusplus
extern "C" {
#endif

struct event {
	TAILQ_ENTRY(event) ev_active_next;
	TAILQ_ENTRY(event) ev_next;
	
	evutil_socket_t ev_fd;

	struct event_base *ev_base;

	union {
		/* used for io events */
		struct {
			TAILQ_ENTRY(event) ev_io_next;
			struct timeval ev_timeout;
		} ev_io;
	} _ev;

	short ev_events;
	short ev_res;		/* result passed to event callback */
	short ev_flags;
	ev_uint8_t ev_pri;	/* smaller numbers are higher priority */
	ev_uint8_t ev_closure;
	struct timeval ev_timeout;

	/* allows us to adopt for different types of events */
	void (*ev_callback)(evutil_socket_t, short, void *arg);
	void *ev_arg;
};

TAILQ_HEAD (event_list, event);

#ifdef __cplusplus
}
#endif

#endif /*  */
