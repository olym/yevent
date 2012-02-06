/*
 * =====================================================================================
 *
 *       Filename:  eventfd.h
 *
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  02/02/2012 10:26:19 PM
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  YOUR NAME (), 
 *        Company:  
 *
 * =====================================================================================
 */
#ifndef _YEVENT_EVENTFD_H
#define _YEVENT_EVENTFD_H
#include <time.h>
struct event;
int create_timerfd();
void set_timerfd(int timerfd, const struct timeval *tv);
int signalfd_create(int signo);
void handle_signal(struct event *ev);
void handle_timeout(struct event *ev);
#endif
