/*
 * =====================================================================================
 *
 *       Filename:  Utility.h
 *
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  04/23/2012 12:26:51 AM
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  YOUR NAME (), 
 *        Company:  
 *
 * =====================================================================================
 */
#ifndef __UTILITY_H
#define __UTILITY_H

namespace yevent {
    namespace util {
        /* Log levels */
#define YEVENT_DEBUG 0
#define YEVENT_VERBOSE 1
#define YEVENT_NOTICE 2
#define YEVENT_WARNING 3
#define YEVENT_MAX_LOGMSG_LEN 4096
        
        int gettid();
        void GetTime(long *seconds, long *milliseconds);
        void AddMillisecondsToNow(long long milliseconds, long *sec, long *ms);
        void yeventLog(int level, const char *fmt, ...);
    }
}
#endif
