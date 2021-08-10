#include "src/base/CurrentThread.h"

namespace CurrentThread {
    __thread int t_cachedTid = 0;
}