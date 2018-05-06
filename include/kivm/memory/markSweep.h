//
// Created by kiva on 2018/4/19.
//
#pragma once

#include <kivm/memory/collectedHeap.h>
#include <kivm/memory/heapRegion.h>

namespace kivm {
    class MarkSweepHeap : public CollectedHeap {
    private:
        jbyte *_memoryStart;
        size_t _totalSize;
        HeapRegion *_firstRegion;
        int _currentRegion;

    private:
        void initializeRegions();

    public:
        MarkSweepHeap();

        ~MarkSweepHeap() override;

        void *allocate(size_t size) override;

        void initializeAll() override;

        inline void *getHeapStart() override {
            return _memoryStart;
        }

        inline void *getHeapEnd() override {
            return _memoryStart + getHeapSize();
        }

        inline size_t getHeapSize() override {
            return _totalSize;
        }

        inline bool isHeapObject(void *addr) override {
            return addr >= _memoryStart && addr < _memoryStart + _totalSize;
        }
    };
}
