/*
 * =====================================================================================
 *
 *       Filename:  epoll.c
 *
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  01/05/2012 10:20:56 AM
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  Yin Zhongxing (olym), zhongxing.yin@cs2c.com.cn
 *        Company:  www.CS2C.com.cn
 *
 * =====================================================================================
 */
#include <stdio.h>
#include <stdlib.h>
#include <sys/epoll.h>
#include <errno.h>

#include <sys/socket.h>
#include <netinet/tcp.h>
#include <netinet/in.h>
#include <unistd.h>
#include <fcntl.h>

#define INITIAL_NEVENT 32
#define MAX_NEVENT 4096

struct epollop {
    struct epoll_event *events;
    int nevents;
    int epfd;
};

void *
epoll_init()
{
    int epfd;
    struct epollop *epollop;

    if ((epfd = epoll_create(32000)) == -1) {
        perror("epoll_create");
        return NULL;
    }

    if (!(epollop = (struct epollop*)malloc(sizeof(struct epollop))))
        return NULL;

    epollop->epfd = epfd;

    epollop->events = (struct epoll_event *)malloc(INITIAL_NEVENT * sizeof(struct epoll_event));
    if (epollop->events == NULL) {
        free(epollop);
        return NULL;
    }
    epollop->nevents = INITIAL_NEVENT;

    return epollop;
}

int
epoll_add(struct event_base* event_base, int fd, int mask)
{
    struct epollop *epollop = event_base->evdata; 
    struct epoll_event ee;
    //ee.events = EPOLLONESHOT;
    if (mask & EV_READ) ee.events |= EPOLLIN;
    if (mask & EV_WRITE) ee.events |= EPOLLOUT;
    if (mask & EV_ET) ee.events |= EPOLLET;
    ee.data.u64 = 0;
    ee.data.fd = fd;
    if (epoll_ctl(epollop->epfd, EPOLL_CTL_ADD, fd, &ee) == -1 && errno != EEXIST) {
        fprintf(stderr, "epoll_ctl(%d,%d) failed: %d\n", EPOLL_CTL_ADD,fd,errno);
        return -1;
    }
    return 0;
}

int
epoll_del(struct event_base *event_base, int fd)
{
    struct epollop *epollop = event_base->evdata;
    struct epoll_event ee;

    ee.events = 0;
    ee.data.fd = fd;
    ee.data.u64 = 0;
    /* Note, Kernel < 2.6.9 requires a non null event pointer even for
     * EPOLL_CTL_DEL. */
    if (epoll_ctl(epollop->epfd, EPOLL_CTL_DEL, fd, &ee) == -1
            && errno != ENOENT && errno != EBADF) {
        fprintf(stderr, "epoll_ctl(%d,%d) failed: %d\n", EPOLL_CTL_DEL,fd,errno);
        return -1;
    }
    return 0;
}

int
epoll_update(struct event_base *event_base, int fd, int mask)
{
    struct epollop *epollop = event_base->evdata;
    struct epoll_event ee;

    //ee.events = EPOLLONESHOT;
    if (mask & EV_READ)
        ee.events |= EPOLLIN;
    if (mask & EV_WRITE)
        ee.events |= EPOLLOUT;
    if (mask & EV_ET)
        ee.events |= EPOLLET;
    ee.data.u64 = 0;
    ee.data.fd = fd;
    if (epoll_ctl(epollop->epfd, EPOLL_CTL_MOD, fd, &ee) == -1 && errno != EEXIST) {
        fprintf(stderr, "epoll_ctl(%d, %d) failed: %d\n", EPOLL_CTL_ADD, fd, errno);
        return -1;
    }
    return 0;
}

void
epoll_free(struct event_base *event_base)
{
    struct epollop *epollop = event_base->evdata;
    close(epollop->epfd);
    free(epollop);
}

int
epoll_dispatch(struct event_base *event_base, struct timeval *tv)
{
    struct epollop *epollop = event_base->evdata;
    int res;

    res = epoll_wait(epollop->epfd, epollop->events, epollop->nevents, 
            tv ? (tv->tv_sec*1000 + tv->tv_usec/1000) : -1);
    int i;
    for (i = 0; i < res; i++) {
        event_base->activefd[i] = epollop->events[i].data.fd;
    }

	if (res == epollop->nevents && epollop->nevents < MAX_NEVENT) {
		/* We used all of the event space this time.  We should
		   be ready for more events next time. */
		int new_nevents = epollop->nevents * 2;
		struct epoll_event *new_events;

		new_events = realloc(epollop->events,
		    new_nevents * sizeof(struct epoll_event));
		if (new_events) {
			epollop->events = new_events;
			epollop->nevents = new_nevents;
		}
	}

    return res;
}

