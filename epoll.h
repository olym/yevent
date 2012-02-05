/*
 * =====================================================================================
 *
 *       Filename:  epoll.h
 *
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  02/02/2012 11:12:55 PM
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  YOUR NAME (), 
 *        Company:  
 *
 * =====================================================================================
 */

#ifndef _YEVENT_EPOLL_H
#define _YEVENT_EPOLL_H

void * epoll_init();
int epoll_add(struct event_base* event_base, int fd, int mask);
int epoll_del(struct event_base *event_base, int fd);
int epoll_update(struct event_base *event_base, int fd, int mask);
void epoll_free(struct event_base *event_base);
int epoll_dispatch(struct event_base *event_base, struct timeval *tv);

#endif
