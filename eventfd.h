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
int create_timerfd();
void set_timerfd(int timerfd, const struct timeval *tv);
int signalfd_creat(int signo);
#endif