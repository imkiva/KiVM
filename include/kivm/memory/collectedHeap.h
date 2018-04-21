//
// Created by kiva on 2018/4/19.
//
#pragma once

#include <kivm/kivm.h>

namespace kivm {
    class CollectedHeap {
    public:
        virtual ~CollectedHeap() = default;

        virtual void *allocate(size_t size) = 0;

        virtual void initializeAll() = 0;

        virtual void* getHeapStart() = 0;

        virtual void *getHeapEnd() = 0;

        virtual size_t getHeapSize() = 0;
    };
}
