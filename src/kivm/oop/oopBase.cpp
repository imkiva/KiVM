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
        memset(ptr, '\0', size);

        // add it to the oopPoll
        if (add_to_pool) {
            LockGuard lg(getOopMemoryLock());
            oopPool::getOopHandlerPool().push_back((oop) ptr);
        }

        return ptr;
    }

    void oopBase::deallocate(void *ptr) {
        if (ptr != nullptr) {
            free(ptr);
        }
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

    void oopBase::cleanup() {
        LockGuard lg(getOopMemoryLock());
        for (auto iter : oopPool::getOopHandlerPool()) {
            delete iter;
        }
    }
}
