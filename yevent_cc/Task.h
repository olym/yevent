/*
 * =====================================================================================
 *
 *       Filename:  Task.h
 *
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  2012年06月14日 15时52分15秒
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  YOUR NAME (), 
 *        Company:  
 *
 * =====================================================================================
 */
#ifndef __TASK_H
#define __TASK_H

namespace yevent {
    typedef void (*TaskFunc)(void *);
    class Task {
        public:
            Task(TaskFunc func, void *args) :
                func_(func),
                args_(args)
        { }
            void runTask() {
                func_(args_);
            }
        private:
            TaskFunc func_; 
            void *args_;
    };
}

#endif
