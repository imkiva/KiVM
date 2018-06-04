//
// Created by kiva on 2018/4/21.
//
#pragma once

#include <kivm/kivm.h>

namespace kivm {
    struct HeapRegion {
        size_t _regionSize;
        jbyte *_regionStart;
        jbyte *_current;

        inline jbyte *getRegionEnd() {
            return _regionStart + _regionSize;
        }

        inline bool shouldAllocate(size_t size) {
            return (_current + size) < getRegionEnd();
        }

        inline void *allocate(size_t size) {
            // bump the pointer
            jbyte *m = _current;
            _current += size;
            return m;
        }
    };
}
