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

namespace yevent
{
    class Acceptor
    {
        public:
            Acceptor();
            ~Acceptor();
            void listen()
            static void handleRead(void *);
            void handleRead();
        private:
            EventLoop *loop_;
            TcpServer *tcpServer_;
            int listenfd_;
            Event acceptEvent_;
            string point_;
            bool isListenning_;
    };
}
