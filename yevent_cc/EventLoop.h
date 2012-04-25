/*
 * =====================================================================================
 *
 *       Filename:  EventLoop.cc
 *
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  04/25/2012 09:55:58 AM
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  Yin Zhongxing (olym), olym.yin@gmail.com
 *        Company:  
 *
 * =====================================================================================
 */
namespace yevent
{
class EventLoop
{
public:
    EventLoop();
    runAt();
    runEvery();
    runAfter();
    cancelTimer(int timerId);
    unregisterEvent();
    registerSignalEvent();
    registerTimerEvent();
    registerEvent();
    dispatch();

private:

    bool stop_;
    int notifyfd_; //notify dispatch wake up
    boost::scoped_ptr<Mutiplexer> mutiplexer_;
    boost::scoped_ptr<MinHeap> minHeap_;
    Event notifyEvent_;
    std::vector<Event> registeredEvents_(YE_MINEVENT);
    std::vector<FiredEvent> fired_(YE_MINEVENT);
    MutexLock mutex_;
};
}
