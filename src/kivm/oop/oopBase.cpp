//
// Created by kiva on 2018/2/25.
//

#include <kivm/oop/oop.h>
#include <cstring>
#include <cstdlib>
#include <kivm/memory/universe.h>

namespace kivm {
    void *GCJavaObject::allocate(size_t size) {
        if (size == 0) {
            return nullptr;
        }

#ifdef KIVM_DEBUG_MALLOC_ONLY
        void *ptr = malloc(size);
#else
        void *ptr = Universe::allocHeap(size);
#endif
        memset(ptr, '\0', size);
        return ptr;
    }

    void GCJavaObject::deallocate(void *ptr) {
#ifdef KIVM_DEBUG_MALLOC_ONLY
        free(ptr);
#endif
    }

    void *GCJavaObject::operator new(size_t size, bool addToPool) noexcept {
        return allocate(size);
    }

    void *GCJavaObject::operator new[](size_t size, bool addToPool) noexcept {
        return allocate(size);
    }

    void GCJavaObject::operator delete(void *ptr) {
        return deallocate(ptr);
    }

    void GCJavaObject::operator delete[](void *ptr) {
        return deallocate(ptr);
    }
}
