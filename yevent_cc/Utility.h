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
        void GetTime(long *seconds, long *milliseconds);
        void AddMillisecondsToNow(long long milliseconds, long *sec, long *ms);
    }
}
#endif
