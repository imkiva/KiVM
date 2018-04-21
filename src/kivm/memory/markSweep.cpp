//
// Created by kiva on 2018/4/20.
//

#include <kivm/memory/markSweep.h>
#include <kivm/memory/universe.h>
#include <kivm/runtime/runtimeConfig.h>
#include <cmath>

namespace kivm {

    MarkSweepHeap::MarkSweepHeap()
        : _memoryStart(nullptr),
          _totalSize(RuntimeConfig::get().initialHeapSizeInBytes),
          _currentRegion(0),
          _firstRegion(nullptr) {
        D("MarkSweepHeap: initialHeapSize: %zd", _totalSize);
    }

    MarkSweepHeap::~MarkSweepHeap() {
        if (_memoryStart != nullptr) {
            Universe::deallocVirtual(_memoryStart);
            _memoryStart = nullptr;
        }
    }

    void *MarkSweepHeap::allocate(size_t size) {
        int currentRegion = _currentRegion;
        HeapRegion *hr = nullptr;
        while (currentRegion < TARGET_REGION_COUNT) {
            hr = _firstRegion + currentRegion;
            if (hr->shouldAllocate(size)) {
                break;
            }
            ++currentRegion;
        }

        if (hr != nullptr) {
            _currentRegion = currentRegion;
            return hr->allocate(size);
        }

        PANIC("OutOfMemoryError: heap");
        return nullptr;
    }

    void MarkSweepHeap::initializeAll() {
        _memoryStart = (jbyte *) Universe::allocVirtual(_totalSize);
        D("MarkSweepHeap: virtual memory allocated: %p", _memoryStart);

        initializeRegions();
    }

    void MarkSweepHeap::initializeRegions() {
        size_t regionSize = std::max((size_t) MIN_REGION_SIZE,
                                          _totalSize / TARGET_REGION_COUNT);
        auto sizeLog = (int) log2(regionSize);
        regionSize = ((size_t) 1) << sizeLog;

        if (regionSize < MIN_REGION_SIZE) {
            regionSize = MIN_REGION_SIZE;
        } else if (regionSize > MAX_REGION_SIZE) {
            regionSize = MAX_REGION_SIZE;
        }

        sizeLog = (int) log2(regionSize);
        regionSize = ((size_t) 1) << sizeLog;

        _firstRegion = (HeapRegion *) Universe::allocCObject(sizeof(HeapRegion) * TARGET_REGION_COUNT);

        jbyte *delivering = _memoryStart;
        HeapRegion *hr = nullptr;
        for (int i = 0; i < TARGET_REGION_COUNT; ++i) {
            hr = _firstRegion + i;
            hr->_regionSize = regionSize;
            hr->_regionStart = delivering;
            hr->_current = delivering;
            delivering += regionSize;
        }
    }
}
