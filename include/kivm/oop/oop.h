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

namespace kivm {

    class GCJavaObject {
    public:
        GCJavaObject() = default;

        virtual ~GCJavaObject() = default;

        static void *allocate(size_t size);

        static void deallocate(void *ptr);

        static void *operator new(size_t size, bool = true) noexcept;

        static void *operator new(size_t size, const std::nothrow_t &) noexcept = delete;
        static void *operator new[](size_t size, bool = true) throw();

        static void *operator new[](size_t size, const std::nothrow_t &) noexcept = delete;
        static void operator delete(void *ptr);

        static void operator delete[](void *ptr);
    };

    class markOopDesc {
    private:
        oopType _type;
        Monitor _monitor;
        jint _hash;

    public:
        markOopDesc(oopType type, oop _oop);

        oopType getOopType() const { return _type; }

        void monitorEnter() {
            _monitor.enter();
            D("MonitorEntered");
        }

        void monitorExit() {
            _monitor.leave();
            D("MonitorExited");
        }

        inline void setHash(jint hash) {
            this->_hash = hash;
        }

        inline jint getHash() const {
            return this->_hash;
        }

        inline void wait() { _monitor.wait(); }

        inline void wait(long macro_sec) { _monitor.wait(macro_sec); }

        inline void notify() { _monitor.notify(); }

        inline void notifyAll() { _monitor.notify_all(); }

        inline void forceUnlockWhenExceptionOccurred() { _monitor.force_unlock_when_athrow(); }
    };

    typedef markOopDesc *markOop;

    class oopDesc : public GCJavaObject {
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
