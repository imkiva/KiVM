//
// Created by kiva on 2018/2/25.
//

#include <kivm/oop/oop.h>
#include <cstring>
#include <cstdlib>

namespace kivm {
    void *oopBase::allocate(size_t size, bool add_to_pool) {
        if (size == 0) {
            return nullptr;
        }

        void *ptr = malloc(size);
        memset(ptr, 0, size);

        // add it to the oopPoll
        if (add_to_pool) {
            LockGuard lg(getOopMemoryLock());
            oopPool::oop_handler_pool().push_back((oop) ptr);
        }

        return ptr;
    }

    void oopBase::deallocate(void *ptr) {
        free(ptr);
    }

    void *oopBase::operator new(size_t size, bool add_to_pool) throw() {
        return allocate(size, add_to_pool);
    }

    void *oopBase::operator new[](size_t size, bool add_to_pool) throw() {
        return allocate(size, add_to_pool);
    }

    void oopBase::operator delete(void *ptr) {
        return deallocate(ptr);
    }

    void oopBase::operator delete[](void *ptr) {
        return deallocate(ptr);
    }

    void oopBase::cleanup() {
        LockGuard lg(getOopMemoryLock());
        for (auto iter : oopPool::oop_handler_pool()) {
            delete iter;
        }
    }
}
