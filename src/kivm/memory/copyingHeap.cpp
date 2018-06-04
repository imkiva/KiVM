//
// Created by kiva on 2018/4/20.
//

#include <kivm/memory/copyingHeap.h>
#include <kivm/memory/universe.h>
#include <kivm/runtime/runtimeConfig.h>
#include <cmath>
#include <kivm/gc/gcThread.h>
#include <kivm/runtime/javaThread.h>

#define REGION_COUNT 2

namespace kivm {
    CopyingHeap::CopyingHeap()
        : _memoryStart(nullptr),
          _totalSize(RuntimeConfig::get().initialHeapSizeInBytes),
          _regions(nullptr),
          _currentRegion(nullptr),
          _nextRegion(nullptr) {
        D("CopyingHeap: initialHeapSize: %zd", _totalSize);
    }

    CopyingHeap::~CopyingHeap() {
        if (_memoryStart != nullptr) {
            Universe::deallocVirtual(_memoryStart);
            _memoryStart = nullptr;
        }
    }

    void *CopyingHeap::allocate(size_t size) {
        if (_currentRegion->shouldAllocate(size)) {
            return _currentRegion->allocate(size);
        }

        // out of memory, let's try GC
        auto currentThread = Threads::currentThread();
        if (currentThread == nullptr) {
            PANIC("OutOfMemoryError: heap (not in JavaThread)");
        }

        D("CopyingHeap: out of memory, will retry after GC");
        auto triggeredMonitor = GCThread::get()->required();

        // this will block current thread until GC is finished
        currentThread->enterSafepoint();

        // GCThread->require() will enter this monitor
        // we should leave this monitor for the coming GC
        if (triggeredMonitor != nullptr) {
            triggeredMonitor->leave();
        }

        // try again
        D("CopyingHeap: retry");
        if (_currentRegion->shouldAllocate(size)) {
            D("CopyingHeap: successfully allocated after GC");
            return _currentRegion->allocate(size);
        }

        PANIC("OutOfMemoryError: heap");
        return nullptr;
    }

    void CopyingHeap::initializeAll() {
        _memoryStart = (jbyte *) Universe::allocVirtual(_totalSize);
        D("CopyingHeap: virtual memory allocated: %p", _memoryStart);

        initializeRegions();
    }

    void CopyingHeap::initializeRegions() {
        if (_totalSize < 2 || ((_totalSize & (_totalSize - 1)) != 0)) {
            PANIC("Heap size should be power of 2");
        }

        size_t regionSize = _totalSize / REGION_COUNT;
        _regions = (HeapRegion *) Universe::allocCObject(sizeof(HeapRegion) * REGION_COUNT);
        _currentRegion = _regions;
        _nextRegion = _regions + 1;

        jbyte *delivering = _memoryStart;

        // setup current and next region
        HeapRegion *hr = nullptr;
        for (int i = 0; i < REGION_COUNT; ++i) {
            hr = _regions + i;
            hr->_regionSize = regionSize;
            hr->_regionStart = delivering;
            hr->_current = delivering;
            delivering += regionSize;
        }
    }
}
