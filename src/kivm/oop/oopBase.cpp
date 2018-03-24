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
            LockGuard lg(mem_lock());
            oopPool::oop_handler_pool().push_back((oop) ptr);
        }

        return ptr;
    }

    void oopBase::deallocate(void *ptr) {
        free(ptr);
    }

    void *oopBase::operator new(size_t size, bool dont_record) throw() {
        return allocate(size, dont_record);
    }

    void *oopBase::operator new[](size_t size, bool dont_record) throw() {
        return allocate(size, dont_record);
    }

    void oopBase::operator delete(void *ptr) {
        return deallocate(ptr);
    }

    void oopBase::operator delete[](void *ptr) {
        return deallocate(ptr);
    }

    void oopBase::cleanup() {
        LockGuard lg(mem_lock());
        for (auto iter : oopPool::oop_handler_pool()) {
            delete iter;
        }
    }
}
