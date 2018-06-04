//
// Created by kiva on 2018/4/19.
//
#pragma once

#include <kivm/memory/collectedHeap.h>
#include <kivm/memory/heapRegion.h>

namespace kivm {
    class CopyingHeap : public CollectedHeap {
    private:
        jbyte *_memoryStart;
        size_t _totalSize;
        HeapRegion *_regions;
        HeapRegion *_currentRegion;
        HeapRegion *_nextRegion;

    private:
        void initializeRegions();

    public:
        CopyingHeap();

        ~CopyingHeap() override;

        void *allocate(size_t size) override;

        void initializeAll() override;

        void doGarbageCollection() override;

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
            return addr >= getHeapStart() && addr < getHeapEnd();
        }
    };
}
