//
// Created by kiva on 2018/4/19.
//
#pragma once

#include <kivm/memory/collectedHeap.h>

namespace kivm {
    class MarkSweepHeap : public CollectedHeap {
    private:
        void *_memoryStart;
        void *_current;
        size_t _size;

    public:
        MarkSweepHeap();

        ~MarkSweepHeap() override = default;

        void *allocate(size_t size) override;

        bool initializeAll() override;

        void *getHeapStart() override {
            return _memoryStart;
        }

        void *getHeapEnd() override {
            return _memoryStart + getHeapSize();
        }

        size_t getHeapSize() override {
            return _size;
        }
    };
}
