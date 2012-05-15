/*
 * =====================================================================================
 *
 *       Filename:  SignalEvent.cc
 *
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  04/26/2012 10:08:29 PM
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  Yin Zhongxing (olym), zhongxing.yin@cs2c.com.cn
 *        Company:  www.CS2C.com.cn
 *
 * =====================================================================================
 */
#include <stdio.h>
#include <sys/signalfd.h>
#include <signal.h>
#include <unistd.h>
#include <stdlib.h>
#include <strings.h>
#include "Event.h"
#include "SignalEvent.h"

using namespace yevent;

using namespace yevent;

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

void SignalEvent::handleEvent()
{
    struct signalfd_siginfo fdsi;
    int s = read(getFd(), &fdsi, sizeof(struct signalfd_siginfo));
    if (s != sizeof(struct signalfd_siginfo))
        fprintf(stderr, "%s: read error \n", __func__);
    printf("Catch signal %d\n", fdsi.ssi_signo);

    if (fdsi.ssi_signo == signo_)
        handleRead();
}
