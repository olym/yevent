/*
 * =====================================================================================
 *
 *       Filename:  SignalEvent.cc
 *
 *    Description:  
 *
 *        Version:  1.0
<<<<<<< HEAD
 *        Created:  04/26/2012 10:08:29 PM
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  Yin Zhongxing (olym), zhongxing.yin@cs2c.com.cn
 *        Company:  www.CS2C.com.cn
=======
 *        Created:  05/07/2012 03:04:35 PM
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  Yin Zhongxing (olym), olym.yin@gmail.com
 *        Company:  
>>>>>>> 3d845658d9de55e180de292ec2b7b9aed07d69eb
 *
 * =====================================================================================
 */

#include "SignalEvent.h"
<<<<<<< HEAD

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
=======
#include <sys/signalfd.h>
#include <signal.h>
#include <unistd.h>
#include <stdlib.h>
#include <strings.h>

using namespace yevent;

virtual SignalEvent::handleEvent()
{
    struct signalfd_siginfo fdsi;
    int s = read(timerfd, &fdsi, sizeof(struct signalfd_siginfo));
>>>>>>> 3d845658d9de55e180de292ec2b7b9aed07d69eb
    if (s != sizeof(struct signalfd_siginfo))
        fprintf(stderr, "%s: read error \n", __func__);
    printf("Catch signal %d\n", fdsi.ssi_signo);

<<<<<<< HEAD
    if (retevent_ & EV_READ){
        if (readCallback_) readCallback_(args_);
    }
=======
    if (fdsi.ssi_signo == signo_)
        readCallback_(evReadArgs); 
>>>>>>> 3d845658d9de55e180de292ec2b7b9aed07d69eb
}
