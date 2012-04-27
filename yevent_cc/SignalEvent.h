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
            SignalEvent(int signo) : Event(signalfd_create(signo), EV_READ) {}
            virtual ~SignalEvent(){}
            virtual handleEvent();
        private:
            SignalEvent();
            int signo_;
    };
    int signalfd_create(int signo);
}
#endif 
