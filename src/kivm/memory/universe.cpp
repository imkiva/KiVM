//
// Created by kiva on 2018/4/20.
//
#include <kivm/memory/universe.h>
#include <kivm/memory/copyingHeap.h>
#include <shared/mmap.h>
#include <cstring>
#include <cerrno>

namespace kivm {
    CollectedHeap *Universe::sCollectedHeapInstance = nullptr;

    struct VirtualMemoryInfo {
        size_t memorySize;
    };

    void Universe::initialize() {
        Universe::sCollectedHeapInstance = new CopyingHeap;
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

        jbyte *memory = nullptr;
        jbyte* FAILURE;

#if  defined(KIVM_PLATFORM_WINDOWS)
        FAILURE = nullptr;
        memory = (jbyte *) VirtualAlloc(nullptr, size,
            MEM_RESERVE, PAGE_READWRITE);
#else
        FAILURE = (jbyte *) MAP_FAILED;
        memory = (jbyte *) mmap(nullptr,
            size + sizeof(VirtualMemoryInfo),
            PROT_READ | PROT_WRITE,
            MAP_ANONYMOUS | MAP_SHARED, -1, 0);
#endif

        if (memory == FAILURE) {
            WARN("Universe::allocVirtual(): failed: %s", strerror(errno));
            return nullptr;
        }

        auto memoryInfo = (VirtualMemoryInfo *) memory;
        memoryInfo->memorySize = size;
        return memory + sizeof(VirtualMemoryInfo);
    }

    void Universe::deallocVirtual(void *memory) {
        jbyte *m = ((jbyte *) memory) - sizeof(VirtualMemoryInfo);
        auto memoryInfo = (VirtualMemoryInfo *) m;
        munmap(m, memoryInfo->memorySize);
    }

    void *Universe::allocHeap(size_t size) {
        if (sCollectedHeapInstance == nullptr) {
            WARN("heap not initialized");
            return nullptr;
        }
        return sCollectedHeapInstance->allocate(size);
    }

    void *Universe::allocCObject(size_t size) {
        void *m = malloc(size);
        if (m == nullptr) {
            WARN("OutOfMemory: system heap");
            return nullptr;
        }
        memset(m, '\0', size);
        return m;
    }

    void Universe::deallocCObject(void *memory) {
        if (memory != nullptr) {
            free(memory);
        }
    }

    bool Universe::isHeapObject(void *addr) {
        if (sCollectedHeapInstance == nullptr) {
            WARN("heap not initialized");
            return false;
        }
        return sCollectedHeapInstance->isHeapObject(addr);
    }
}
