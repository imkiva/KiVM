//
// Created by kiva on 2018/2/25.
//

#ifndef KIVAVM_OOP_H
#define KIVAVM_OOP_H

#include <kivm/kivm.h>
#include <kivm/klass.h>
#include <shared/lock.h>
#include <shared/monitor.h>

#include <list>

// Forward declaration
class oopDesc;

typedef oopDesc *oop;

class oopPool {
public:
    static std::list<oop> &oop_handler_pool() {
        static std::list<oop> oop_handler_pool;
        return oop_handler_pool;
    }

    static oop copy(oop _oop);
};

class oopBase {
private:
    DISALLOW_COPY(oopBase);

    static Lock &mem_lock() {
        static Lock mem_lock;
        return mem_lock;
    }

public:
    oopBase() = default;

    virtual ~oopBase() = default;

    static void *allocate(size_t size, bool add_to_pool);

    static void deallocate(void *ptr);

    static void *operator new(size_t size, bool = false) throw();

    static void *operator new(size_t size, const std::nothrow_t &) throw() { exit(-2); }        // do not use it.
    static void *operator new[](size_t size, bool = false) throw();

    static void *operator new[](size_t size, const std::nothrow_t &) throw() { exit(-2); }        // do not use it.
    static void operator delete(void *ptr);

    static void operator delete[](void *ptr);

    static void cleanup();
};

class markOopDesc {
private:
    oopType _type;
    Monitor _monitor;

public:
    oopType type() const { return _type; }

    void enter_monitor() { _monitor.enter(); }

    void leave_monitor() { _monitor.leave(); }

    void wait() { _monitor.wait(); }

    void wait(long macro_sec) { _monitor.wait(macro_sec); }

    void notify() { _monitor.notify(); }

    void notify_all() { _monitor.notify_all(); }

    void force_unlock_when_athrow() { _monitor.force_unlock_when_athrow(); }
};

typedef markOopDesc *markOop;

class oopDesc : public oopBase {
private:
    markOop _mark;
    Klass *_klass;

public:
    explicit oopDesc(markOop mark, Klass *klass) : _mark(mark), _klass(klass) {}

    explicit oopDesc(oop other) : _mark(other->mark()), _klass(other->klass()) {}

    virtual ~oopDesc();

    virtual oop copy();

    markOop mark() { return _mark; }

    Klass *klass() { return _klass; }
};

#endif //KIVAVM_OOP_H
