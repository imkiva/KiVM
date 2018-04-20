//
// Created by kiva on 2018/4/20.
//

#include <kivm/memory/markSweep.h>
#include <kivm/memory/universe.h>
#include <kivm/runtime/runtimeConfig.h>

namespace kivm {

    MarkSweepHeap::MarkSweepHeap()
        : _memoryStart(nullptr), _current(nullptr),
          _size(RuntimeConfig::get().initialHeapSizeInBytes) {
    }

    MarkSweepHeap::~MarkSweepHeap() {
        if (_memoryStart != nullptr) {
            delete _memoryStart;
            _memoryStart = nullptr;
            _current = nullptr;
        }
    }

    void *MarkSweepHeap::allocate(size_t size) {
        // bump the pointer
        if (_current + size < getHeapEnd()) {
            void *m = _current;
            _current += size;
            return m;
        }
        PANIC("OutOfMemoryError: heap");
        return nullptr;
    }

    bool MarkSweepHeap::initializeAll() {
        _memoryStart = Universe::allocVirtual(_size);
        if (_memoryStart != nullptr) {
            Universe::memoryZero(_memoryStart, _size);
            _current = _memoryStart;
            return true;
        }
        return false;
    }
}
