#ifndef TIMESTAMP_H
#define TIMESTAMP_H

#include <sys/types.h>
#include <string>

using namespace std;

class Timestamp {
public:
    Timestamp();
    Timestamp(int64_t microSeconds);
    ~Timestamp() = default;

    bool valid() const { return microSecondsSinceEpoch_>0; };

    int64_t microSecondsSinceEpoch() const { return microSecondsSinceEpoch_; };

    string toString() const;
    
    static Timestamp now();

    static Timestamp nowAfter(double seconds);
    
    static const int kMicroSecondsPerSecond = 1000 * 1000;

    // TODO: format to string 

private:

    int64_t microSecondsSinceEpoch_;

};

inline bool operator<(Timestamp t1, Timestamp t2) {
    return t1.microSecondsSinceEpoch() < t2.microSecondsSinceEpoch();
}

inline bool operator==(Timestamp t1, Timestamp t2) {
    return t1.microSecondsSinceEpoch() == t2.microSecondsSinceEpoch();
}

#endif