/*
 * =====================================================================================
 *
 *       Filename:  TimerManager.cc
 *
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  04/27/2012 03:51:15 PM
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  Yin Zhongxing (olym), olym.yin@gmail.com
 *        Company:  
 *
 * =====================================================================================
 */

using namespace yevent;

virtual void TimerEvent::handleEvent()
{
    uint64_t exp;
    int s = ::read(signalfd, &exp, sizeof(uint64_t));
    if (s != sizeof(uint64_t))
        fprintf(stderr, "%s: read error \n", __func__);

    Timer *timer = getTimer(currentTimerId_);

    timer->timerCb();

    if (timer->timerType == TimerEvent::OnceTimer)
        deleteTimer(currentTimerId_);
    else
        reAddTimer(currentTimerId_);

    Timer *timer = getNearestTimer();
    if (timer != NULL)
        setTimerfd(timer->when_sec, timer->when_ms);
    else
        m_pLoop_->deleteTimerEvent();
}

int CreateTimerfd()
{
    int timerfd = timerfd_create(CLOCK_MONOTONIC, TFD_NONBLOCK | TFD_CLOEXEC);

    if (timerfd < 0) {
        fprintf(stderr, "failed in timerfd_create\n");
        return -1;
    }

    return timerfd;
}
