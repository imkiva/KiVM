//
// Created by kiva on 2018/4/20.
//
#include <kivm/memory/universe.h>
#include <kivm/memory/markSweep.h>
#include <sys/mman.h>
#include <cstring>
#include <cerrno>

namespace kivm {
    CollectedHeap *Universe::sCollectedHeapInstance = nullptr;

    struct VirtualMemoryInfo {
        size_t memorySize;
    };

    void Universe::initialize() {
        Universe::sCollectedHeapInstance = new MarkSweepHeap;
        Universe::sCollectedHeapInstance->initializeAll();
    }

    void Universe::destroy() {
        if (Universe::sCollectedHeapInstance != nullptr) {
            delete Universe::sCollectedHeapInstance;
            Universe::sCollectedHeapInstance = nullptr;
        }
    }

    void *Universe::allocVirtual(size_t size) {
        D("allocVirtual: %zd", size);
        auto m = (jbyte *) mmap(nullptr, size + sizeof(VirtualMemoryInfo),
                                PROT_READ | PROT_WRITE,
                                MAP_ANONYMOUS | MAP_SHARED, -1, 0);
        if (m == MAP_FAILED) {
            PANIC("mmap() failed: %s", strerror(errno));
            return nullptr;
        }

        auto memoryInfo = (VirtualMemoryInfo *) m;
        memoryInfo->memorySize = size;
        return m + sizeof(VirtualMemoryInfo);
    }

    void Universe::deallocVirtual(void *memory) {
        jbyte *m = ((jbyte *) memory) - sizeof(VirtualMemoryInfo);
        auto memoryInfo = (VirtualMemoryInfo *) m;
        munmap(m, memoryInfo->memorySize);
    }

    void *Universe::allocHeap(size_t size) {
        if (sCollectedHeapInstance == nullptr) {
            PANIC("heap not initialized");
        }
        return sCollectedHeapInstance->allocate(size);
    }

    void *Universe::allocCObject(size_t size) {
        void *m = malloc(size);
        if (m == nullptr) {
            PANIC("OutOfMemory: system heap");
        }
        return m;
    }

    void Universe::deallocCObject(void *memory) {
        if (memory != nullptr) {
            free(memory);
        }
    }
}
