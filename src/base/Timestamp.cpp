#include "Timestamp.h"
#include <sys/time.h>
#include <stdio.h>
#define __STDC_FORMAT_MACROS
#include <inttypes.h>
#undef __STDC_FORMAT_MACROS


Timestamp::Timestamp(): microSecondsSinceEpoch_(0)
{}

Timestamp::Timestamp(int64_t microSeconds) : microSecondsSinceEpoch_(microSeconds) 
{}


string Timestamp::toString() const {
    char buf[32] = {0};
    int64_t seconds = microSecondsSinceEpoch_ / kMicroSecondsPerSecond;
    int64_t microSeconds = microSecondsSinceEpoch_ % kMicroSecondsPerSecond;
    snprintf(buf, sizeof(buf), "%" PRId64 ".%06" PRId64 "", seconds, microSeconds);
    return buf;
}

Timestamp Timestamp::now() {
    struct timeval tv;
    gettimeofday(&tv, NULL);
    return Timestamp(tv.tv_sec*kMicroSecondsPerSecond + tv.tv_usec);
}

Timestamp Timestamp::nowAfter(double seconds) {
    int64_t addSeconds = static_cast<int64_t>(seconds);
    int64_t addMicroSeconds = static_cast<int64_t>((seconds - static_cast<double>(addSeconds))*kMicroSecondsPerSecond);
    struct timeval tv;
    gettimeofday(&tv, NULL);
    int64_t seconds_ = addSeconds + tv.tv_sec;
    int64_t mSeconds_ = addMicroSeconds + tv.tv_usec;
    return Timestamp(seconds_*kMicroSecondsPerSecond + mSeconds_);
}


