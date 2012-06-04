/*
 * =====================================================================================
 *
 *       Filename:  Timestamp.h
 *
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  05/07/2012 10:05:42 AM
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  YOUR NAME (), 
 *        Company:  
 *
 * =====================================================================================
 */
#ifndef _TIMESTAMP_H
#define _TIMESTAMP_H

#include <stdint.h>
#include <string>

namespace yevent
{

class Timestamp
{
 public:
  Timestamp()
    : microSecondsSinceEpoch_(0)
  {
  }

  explicit Timestamp(double timeout);
  explicit Timestamp(int64_t microSecondsSinceEpoch);

  void swap(Timestamp& that)
  {
    std::swap(microSecondsSinceEpoch_, that.microSecondsSinceEpoch_);
  }

  std::string toString() const;
  std::string toFormattedString() const;

  bool valid() const { return microSecondsSinceEpoch_ > 0; }

  int64_t microSecondsSinceEpoch() const { return microSecondsSinceEpoch_; }
  time_t secondsSinceEpoch() const
  { return static_cast<time_t>(microSecondsSinceEpoch_ / kMicroSecondsPerSecond); }

  
  static Timestamp now();
  static Timestamp invalid();

  static const int kMicroSecondsPerSecond = 1000 * 1000;

 private:
  int64_t microSecondsSinceEpoch_;
};

inline bool operator<(Timestamp lhs, Timestamp rhs)
{
  return lhs.microSecondsSinceEpoch() < rhs.microSecondsSinceEpoch();
}

inline bool operator>(Timestamp lhs, Timestamp rhs)
{
  return lhs.microSecondsSinceEpoch() > rhs.microSecondsSinceEpoch();
}
inline bool operator==(Timestamp lhs, Timestamp rhs)
{
  return lhs.microSecondsSinceEpoch() == rhs.microSecondsSinceEpoch();
}

inline double timeDifference(Timestamp high, Timestamp low)
{
  int64_t diff = high.microSecondsSinceEpoch() - low.microSecondsSinceEpoch();
  return static_cast<double>(diff) / Timestamp::kMicroSecondsPerSecond;
}

inline Timestamp addTime(Timestamp timestamp, double seconds)
{
  int64_t delta = static_cast<int64_t>(seconds * Timestamp::kMicroSecondsPerSecond);
  return Timestamp(timestamp.microSecondsSinceEpoch() + delta);
}
inline void TimestampToTimeval(Timestamp timestamp, long *sec, long *usec)
{
    int64_t diff =  timestamp.microSecondsSinceEpoch() - Timestamp::now().microSecondsSinceEpoch();
    *sec = diff < 0 ? 0:diff / Timestamp::kMicroSecondsPerSecond;
    *usec = diff < 0 ? 0: diff % Timestamp::kMicroSecondsPerSecond;
}

}

#endif
