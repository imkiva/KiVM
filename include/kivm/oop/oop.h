//
// Created by kiva on 2018/2/25.
//

#pragma once

#include <kivm/kivm.h>
#include <kivm/oop/oopfwd.h>
#include <kivm/oop/klass.h>
#include <shared/lock.h>
#include <shared/monitor.h>
#include <list>

// Forward declaration

namespace kivm {
    class oopPool {
    public:
        static std::list<oop> &getOopHandlerPool() {
            static std::list<oop> handlerPool;
            return handlerPool;
        }
    };

    class oopBase {
    private:
        static Lock &getOopMemoryLock() {
            static Lock mem_lock;
            return mem_lock;
        }

    public:
        oopBase() = default;

        virtual ~oopBase() = default;

        static void *allocate(size_t size, bool add_to_pool);

        static void deallocate(void *ptr);

        static void *operator new(size_t size, bool = true) noexcept;

        static void *operator new(size_t size, const std::nothrow_t &) noexcept { exit(-2); }        // do not use it.
        static void *operator new[](size_t size, bool = true) throw();

        static void *operator new[](size_t size, const std::nothrow_t &) noexcept { exit(-2); }        // do not use it.
        static void operator delete(void *ptr);

        static void operator delete[](void *ptr);

        static void cleanup();
    };

    class markOopDesc {
    private:
        oopType _type;
        Monitor _monitor;

    public:
        markOopDesc(oopType type);

        oopType getOopType() const { return _type; }

        void enterMonitor() { _monitor.enter(); }

        void leaveMonitor() { _monitor.leave(); }

        void wait() { _monitor.wait(); }

        void wait(long macro_sec) { _monitor.wait(macro_sec); }

        void notify() { _monitor.notify(); }

        void notifyAll() { _monitor.notify_all(); }

        void forceUnlockWhenExceptionOccurred() { _monitor.force_unlock_when_athrow(); }
    };

    typedef markOopDesc *markOop;

    class oopDesc : public oopBase {
    private:
        markOop _mark;
        Klass *_klass;

    public:
        explicit oopDesc(Klass *klass, oopType type);

        ~oopDesc() override;

        markOop getMarkOop() const { return _mark; }

        Klass *getClass() const { return _klass; }
    };
}
