/*
 * =====================================================================================
 *
 *       Filename:  EventLoopThreadPool.h
 *
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  04/26/2012 12:47:29 AM
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
    class ThreadPool {
        public:
            ThreadPool();
            ~ThreadPool();


            void start(int numThreads);
            void stop();

            void run(const Task& f);

        private:
            void runInThread();
            Task take();

            MutexLock mutex_;
            Condition cond_;
            boost::ptr_vector<muduo::Thread> threads_;
            std::deque<Task> queue_;
            bool running_;
    };
}
