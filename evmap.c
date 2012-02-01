/*
 * =====================================================================================
 *
 *       Filename:  evmap.c
 *
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  01/31/2012 05:28:27 PM
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  Yin Zhongxing (olym), zhongxing.yin@cs2c.com.cn
 *        Company:  www.CS2C.com.cn
 *
 * =====================================================================================
 */
#include <sys/types.h>
#include <sys/queue.h>
#include <stdlib.h>
#include <unistd.h>
#include <stdio.h>
#include <errno.h>

#include "event-internal.h"
/** An entry for an evmap_io list: notes all the events that want to read or
	write on a given fd, and the number of each.
  */
struct evmap_io {
	struct event_list events;
	ev_uint16_t nread;
	ev_uint16_t nwrite;
};

#define GET_SIGNAL_SLOT(x, map, slot, type)			\
	(x) = (struct type *)((map)->entries[slot])
#define GET_IO_SLOT_AND_CTOR(x, map, slot, type, ctor)	\
	do {								\
		if ((map)->entries[slot] == NULL) {			\
			(map)->entries[slot] =				\
			    calloc(1,sizeof(struct type)); \
			if ((map)->entries[slot] == NULL) \
				return (-1);				\
			(ctor)((struct type *)(map)->entries[slot]);	\
		}							\
		(x) = (struct type *)((map)->entries[slot]);		\
	} while (0)

/** Expand 'map' with new entries of width 'msize' until it is big enough
	to store a value in 'slot'.
 */
static int
evmap_make_space(struct event_io_map *map, int slot, int msize)
{
	if (map->nentries <= slot) {
		int nentries = map->nentries ? map->nentries : 32;
		void **tmp;

		while (nentries <= slot)
			nentries <<= 1;

		tmp = (void **)realloc(map->entries, nentries * msize);
		if (tmp == NULL)
			return (-1);

		memset(&tmp[map->nentries], 0,
		    (nentries - map->nentries) * msize);

		map->nentries = nentries;
		map->entries = tmp;
	}

	return (0);
}
static void evmap_io_init(struct evmap_io *entry)
{
    TAILQ_INIT(&entry->events);
    entry->nread = 0;
    entry->nwrite = 0;
}

int evmap_io_add(struct event_base *base, int fd, struct event *ev)
{
    struct event_io_map *map = &base->iomap;
    struct evmap_io *ctx = NULL;
	int nread, nwrite, retval = 0;
	short res = 0, old = 0;
	struct event *old_ev;

	if (fd < 0)
		return 0;

    if (fd >= map->nentries) {
        if (evmap_make_space(map, fd, sizeof(struct evmap_io *)) == -1)
            return -1;
    }

    GET_IO_SLOT_AND_CTOR(ctx, map, fd, evmap_io, evmap_io_init);

	nread = ctx->nread;
	nwrite = ctx->nwrite;

	if (nread)
		old |= EV_READ;
	if (nwrite)
		old |= EV_WRITE;

	if (ev->ev_events & EV_READ) {
		if (++nread == 1)
			res |= EV_READ;
	}
	if (ev->ev_events & EV_WRITE) {
		if (++nwrite == 1)
			res |= EV_WRITE;
	}
    if (res) {
        if (epoll_add(base, ev->ev_fd, old, ev->events) == -1)
            return -1;
        retval = 1;
    }

	ctx->nread = (uint16_t) nread;
	ctx->nwrite = (uint16_t) nwrite;
	TAILQ_INSERT_TAIL(&ctx->events, ev, ev_io_next);

	return (1);
}

int evmap_io_del(struct event_base *base, int fd, struct event *ev)
{
	struct event_io_map *io = &base->iomap;
	struct evmap_io *ctx;
	int nread, nwrite, retval = 0;
	short res = 0, old = 0;

	if (fd < 0)
		return 0;

	if (fd >= io->nentries)
		return (-1);

	GET_IO_SLOT(ctx, io, fd, evmap_io);

	nread = ctx->nread;
	nwrite = ctx->nwrite;

	if (nread)
		old |= EV_READ;
	if (nwrite)
		old |= EV_WRITE;

	if (ev->ev_events & EV_READ) {
		if (--nread == 0)
			res |= EV_READ;
		assert(nread >= 0);
	}
	if (ev->ev_events & EV_WRITE) {
		if (--nwrite == 0)
			res |= EV_WRITE;
		assert(nwrite >= 0);
	}

	if (res) {
		if (epoll_del(base, ev->ev_fd, old, res) == -1)
			return (-1);
		retval = 1;
	}

	ctx->nread = nread;
	ctx->nwrite = nwrite;
	TAILQ_REMOVE(&ctx->events, ev, ev_io_next);

	return (retval);
}

int evmap_io_active(struct event_base *base, int fd, short events)
{
	struct event_io_map *io = &base->iomap;
	struct evmap_io *ctx;
	struct event *ev;

	GET_IO_SLOT(ctx, io, fd, evmap_io);

	assert(ctx);
	TAILQ_FOREACH(ev, &ctx->events, ev_io_next) {
		if (ev->ev_events & events)
			event_active_nolock(ev, ev->ev_events & events, 1);
	}

}

