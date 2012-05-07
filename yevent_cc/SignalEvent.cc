/*
 * =====================================================================================
 *
 *       Filename:  SignalEvent.cc
 *
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  05/07/2012 03:04:35 PM
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  Yin Zhongxing (olym), olym.yin@gmail.com
 *        Company:  
 *
 * =====================================================================================
 */

#include "SignalEvent.h"
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
    if (s != sizeof(struct signalfd_siginfo))
        fprintf(stderr, "%s: read error \n", __func__);
    printf("Catch signal %d\n", fdsi.ssi_signo);

    if (fdsi.ssi_signo == signo_)
        readCallback_(evReadArgs); 
}
