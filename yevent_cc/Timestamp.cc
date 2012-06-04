/*
 * =====================================================================================
 *
 *       Filename:  Timestamp.cc
 *
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  05/07/2012 10:24:16 AM
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  Yin Zhongxing (olym), olym.yin@gmail.com
 *        Company:  
 *
 * =====================================================================================
 */

#include "Timestamp.h"
#include <sys/time.h>
#include <stdio.h>
#include <inttypes.h>


using namespace yevent;
using std::string;

Timestamp::Timestamp(int64_t microseconds)
  : microSecondsSinceEpoch_(microseconds)
{
}

Timestamp::Timestamp(double timeout)
    :microSecondsSinceEpoch_(0)
{

  struct timeval tv;
  gettimeofday(&tv, NULL);
  microSecondsSinceEpoch_ = static_cast<int64_t>(tv.tv_sec) * kMicroSecondsPerSecond + tv.tv_usec + static_cast<int64_t>(timeout) * kMicroSecondsPerSecond;
}
string Timestamp::toString() const
{
  char buf[32] = {0};
  int64_t seconds = microSecondsSinceEpoch_ / kMicroSecondsPerSecond;
  int64_t microseconds = microSecondsSinceEpoch_ % kMicroSecondsPerSecond;
  snprintf(buf, sizeof(buf)-1, "%lld.%06lld", seconds, microseconds);
  return buf;
}

string Timestamp::toFormattedString() const
{
  char buf[32] = {0};
  time_t seconds = static_cast<time_t>(microSecondsSinceEpoch_ / kMicroSecondsPerSecond);
  int microseconds = static_cast<int>(microSecondsSinceEpoch_ % kMicroSecondsPerSecond);
  struct tm tm_time;
  gmtime_r(&seconds, &tm_time);

  snprintf(buf, sizeof(buf), "%4d-%02d-%02d %02d:%02d:%02d.%06d",
      tm_time.tm_year + 1900, tm_time.tm_mon + 1, tm_time.tm_mday,
      tm_time.tm_hour, tm_time.tm_min, tm_time.tm_sec,
      microseconds);
  return buf;
}

Timestamp Timestamp::now()
{
  struct timeval tv;
  gettimeofday(&tv, NULL);
  int64_t seconds = tv.tv_sec;
  return Timestamp(seconds * kMicroSecondsPerSecond + tv.tv_usec);
}

Timestamp Timestamp::invalid()
{
  return Timestamp();
}
