//
// Created by kiva on 2018/4/20.
//
#include <kivm/memory/universe.h>
#include <kivm/memory/markSweep.h>
#include <cstring>

namespace kivm {
    CollectedHeap * Universe::sCollectedHeapInstance = nullptr;

    void Universe::initialize() {
        Universe::sCollectedHeapInstance = new MarkSweepHeap;
        if (!Universe::sCollectedHeapInstance->initializeAll()) {
            PANIC("CollectedHeap initialization failed");
        }
    }

    void *Universe::allocVirtual(size_t size) {
        return nullptr;
    }

    void *Universe::deallocVirtual(void *memory, size_t size) {
        return nullptr;
    }
}
