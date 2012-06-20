/*
 * =====================================================================================
 *
 *       Filename:  Utility.cc
 *
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  04/27/2012 03:26:54 PM
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  Yin Zhongxing (olym), olym.yin@gmail.com
 *        Company:  
 *
 * =====================================================================================
 */

#include <stdio.h>
#include <stdlib.h>
#include <sys/time.h>
#include <sys/types.h>
#include <unistd.h>
#include <stdarg.h>
#include <syslog.h>
#include <time.h>
#include <sys/syscall.h>
#include "Utility.h"

namespace yevent {
namespace util {
char *logfile = NULL;
int syslog_enabled = 0;
int verbosity = YEVENT_DEBUG;

//Glibc does not provide a wrapper for this system call; call it using syscall
int gettid()
{
    return ::syscall(SYS_gettid);
}

void yeventLog(int level, const char *fmt, ...) {
    const int syslogLevelMap[] = { LOG_DEBUG, LOG_INFO, LOG_NOTICE, LOG_WARNING };
    const char *c = ".-*#";
    time_t now = time(NULL);
    va_list ap;
    FILE *fp;
    char buf[64];
    char msg[YEVENT_MAX_LOGMSG_LEN];

    if (level < verbosity) return;

    fp = (logfile == NULL) ? stdout : fopen(logfile,"a");
    if (!fp) return;

    va_start(ap, fmt);
    vsnprintf(msg, sizeof(msg), fmt, ap);
    va_end(ap);

    strftime(buf,sizeof(buf),"%d %b %H:%M:%S",localtime(&now));
    fprintf(fp,"[%d] %s %c %s\n",gettid(),buf,c[level],msg);
    fflush(fp);

    if (logfile) fclose(fp);

    if (syslog_enabled) syslog(syslogLevelMap[level], "%s", msg);
}

void GetTime(long *seconds, long *milliseconds)
{
    struct timeval tv;

    gettimeofday(&tv, NULL);
    *seconds = tv.tv_sec;
    *milliseconds = tv.tv_usec/1000;
}

void AddMillisecondsToNow(long long milliseconds, long *sec, long *ms) {
    long cur_sec, cur_ms, when_sec, when_ms;

    GetTime(&cur_sec, &cur_ms);
    when_sec = cur_sec + milliseconds/1000;
    when_ms = cur_ms + milliseconds%1000;
    if (when_ms >= 1000) {
        when_sec ++;
        when_ms -= 1000;
    }
    *sec = when_sec;
    *ms = when_ms;
}
}
}

