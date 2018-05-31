//
// Created by kiva on 2018/6/1.
//

#include <kivm/runtime/integerCache.h>

namespace kivm {
    intOop IntegerCache::sIntegerPool[INTEGER_CACHE_MAX] = {nullptr};

    void IntegerCache::initialize() {
        jint k = INTEGER_CACHE_LOW;
        for (int i = 0; i < INTEGER_CACHE_MAX; ++i) {
            IntegerCache::sIntegerPool[i] = new intOopDesc(k);
        }
    }

    intOop IntegerCache::valueOf(jint value) {
        if (value >= INTEGER_CACHE_LOW && value <= INTEGER_CACHE_HIGH) {
            return IntegerCache::sIntegerPool[value + (-INTEGER_CACHE_LOW)];
        }
        return new intOopDesc(value);
    }
}
