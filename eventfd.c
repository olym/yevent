/*
 * =====================================================================================
 *
 *       Filename:  eventfd.c
 *
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  01/31/2012 05:20:50 PM
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  Yin Zhongxing (olym), zhongxing.yin@cs2c.com.cn
 *        Company:  www.CS2C.com.cn
 *
 * =====================================================================================
 */
#include <sys/timerfd.h>
#include <sys/signalfd.h>
#include <signal.h>
#include <time.h>
#include <unistd.h>
#include <stdlib.h>
#include <strings.h>
#include <stdio.h>
#include <stdint.h>
#include "eventfd.h"
//TODO
#include "event-internal.h"

int create_timerfd()
{
    int timerfd = timerfd_create(CLOCK_MONOTONIC, TFD_NONBLOCK | TFD_CLOEXEC);

    if (timerfd < 0) {
        fprintf(stderr, "failed in timerfd_create\n");
        return -1;
    }

    return timerfd;
}

void set_timerfd(int timerfd, const struct timeval *tv)
{
  // wake up loop by timerfd_settime()
  struct itimerspec newValue;
  struct itimerspec oldValue;
  bzero(&newValue, sizeof newValue);
  bzero(&oldValue, sizeof oldValue);
  newValue.it_value.tv_sec = tv->tv_sec;
  newValue.it_value.tv_nsec = tv->tv_usec/1000;
  int ret = timerfd_settime(timerfd, 0, &newValue, &oldValue);
  if (ret) {
      fprintf(stderr, "%s: timerfd_settime()", __func__);
  }
}

int signalfd_create(int signo)
{
    sigset_t mask;
    int sfd;

    sigemptyset(&mask);
    sigaddset(&mask, signo);

    if (sigprocmask(SIG_BLOCK, &mask, NULL) == -1) {
        fprintf(stderr, "%s: sigprocmask\n", __func__);
        return -1;
    }

    sfd = signalfd(-1, &mask, SFD_CLOEXEC | SFD_NONBLOCK);
    if (sfd == -1) {
        fprintf(stderr, "%s: signalfd\n", __func__);
        return -1;
    }

    return sfd;
}

void read_signalfd(int signalfd)
{
    struct signalfd_siginfo fdsi;
    int s = read(signalfd, &fdsi, sizeof(struct signalfd_siginfo));
    if (s != sizeof(struct signalfd_siginfo))
        fprintf(stderr, "%s: read error \n", __func__);
    printf("Catch signal %d\n", fdsi.ssi_signo);
}
void read_timerfd(int signalfd)
{
    uint64_t exp;
    int s = read(signalfd, &exp, sizeof(uint64_t));
    if (s != sizeof(uint64_t))
        fprintf(stderr, "%s: read error \n", __func__);
}
