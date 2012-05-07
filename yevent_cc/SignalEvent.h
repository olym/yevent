/*
 * =====================================================================================
 *
 *       Filename:  SignalEvent.h
 *
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  04/26/2012 12:09:58 AM
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  YOUR NAME (), 
 *        Company:  
 *
 * =====================================================================================
 */
#ifndef __SIGNAL_EVENT_H
#define __SIGNAL_EVENT_H

namespace yevent
{
    class SignalEvent : public Event
    {
        public:
            SignalEvent(EventLoop *loop, int signo) : Event(loop, signalfd_create(signo), EV_READ) {}
            virtual ~SignalEvent(){}
            virtual void handleEvent();
        private:
            SignalEvent();
            int signo_;
    };
    int signalfd_create(int signo);
}
#endif 
