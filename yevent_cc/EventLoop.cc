/*
 * =====================================================================================
 *
 *       Filename:  EventLoop.cc
 *
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  04/25/2012 09:55:32 PM
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  Yin Zhongxing (olym), zhongxing.yin@cs2c.com.cn
 *        Company:  www.CS2C.com.cn
 *
 * =====================================================================================
 */
using namespace yevent;

EventLoop::registerSignalEvent(int signal, EventCallback cb, void *arg)
{
    int sfd = signalfd_create(signal);
    Event *ev = new Event(sfd, EV_READ);
    ev->setReadCallback(cb, arg);
    
    registerEvent(ev);
}
