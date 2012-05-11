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

#include "SignalEvent.h"

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

virtual void SignalEvent::handEvent()
{
    struct signalfd_siginfo fdsi;
    int s = read(fd_, &fdsi, sizeof(struct signalfd_siginfo));
    if (s != sizeof(struct signalfd_siginfo))
        fprintf(stderr, "%s: read error \n", __func__);
    printf("Catch signal %d\n", fdsi.ssi_signo);

    if (retevent_ & EV_READ){
        if (readCallback_) readCallback_(args_);
    }
}
