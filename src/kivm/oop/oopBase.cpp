//
// Created by kiva on 2018/2/25.
//

#include <kivm/oop/oop.h>
#include <cstring>
#include <cstdlib>
#include <kivm/memory/universe.h>

namespace kivm {
    void *oopBase::allocate(size_t size, bool addToPool) {
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

    void oopBase::deallocate(void *ptr) {
#ifdef KIVM_DEBUG_MALLOC_ONLY
        free(ptr);
#else
        // TODO: garbage collector
#endif
    }

    void *oopBase::operator new(size_t size, bool addToPool) throw() {
        return allocate(size, addToPool);
    }

    void *oopBase::operator new[](size_t size, bool addToPool) throw() {
        return allocate(size, addToPool);
    }

    void oopBase::operator delete(void *ptr) {
        return deallocate(ptr);
    }

    void oopBase::operator delete[](void *ptr) {
        return deallocate(ptr);
    }
}
