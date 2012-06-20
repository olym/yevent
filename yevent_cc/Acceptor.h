/*
 * =====================================================================================
 *
 *       Filename:  Acceptor.h
 *
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  04/26/2012 12:41:13 AM
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  YOUR NAME (), 
 *        Company:  
 *
 * =====================================================================================
 */

#ifndef ___ACCEPTOR_H
#define ___ACCEPTOR_H

namespace yevent
{
    class TcpServer;
    class EventLoop;

    class Acceptor
    {
        public:
            Acceptor(EventLoop *loop, TcpServer *server, int port);
            ~Acceptor();
            void listen();
            static void handleRead(void *);
            void handleRead();
        private:
            EventLoop *loop_;
            TcpServer *server_;
            int listenfd_;
            int idleFd_;
            Event acceptEvent_;
            int port_;
            bool isListenning_;
    };
}
#endif 
