//
// Created by kiva on 2018/3/23.
//
#pragma once

#include <kivm/oop/instanceOop.h>
#include <kivm/runtime/stack.h>
#include <kivm/runtime/frame.h>
#include <list>
#include <thread>

namespace kivm {
    enum ThreadState {
        RUNNING, BLOCKED, DIED
    };

    class Thread {
    protected:
        instanceOop _java_thread_object;
        std::thread *_native_thread;

        ThreadState _state;
        Method *_method;
        FrameList _frames;
        std::list<oop> _args;
        u8 *_pc;

        virtual void start() = 0;

        virtual bool should_record_in_thread_table();

    public:
        Thread(Method *method, const std::list<oop> &args);

        virtual ~Thread();

        void create(instanceOop java_thread);

        virtual void thread_lunched();

        inline instanceOop get_java_thread_object() const {
            return _java_thread_object;
        }

        ThreadState get_state() const {
            return _state;
        }

        void set_state(ThreadState _state) {
            Thread::_state = _state;
        }
    };

    // The Java app thread
    class JavaThread : public Thread {
        friend class Execution;

    public:
        JavaThread(Method *method, const std::list<oop> &args);

    protected:
        void start() override;

        oop run_method(Method *method, const std::list<oop> &args);
    };

    // The Java main thread
    class JavaMainThread : public JavaThread {
    public:
        JavaMainThread();

    protected:
        void start() override;

        void thread_lunched() override;

        bool should_record_in_thread_table() override;
    };

    class Threads {
    private:
        static int &app_thread_count() {
            static int app_thread_count;
            return app_thread_count;
        }

        static std::list<Thread *> app_threads() {
            static std::list<Thread *> app_threads;
            return app_threads;
        }

    public:
        static void initializeJVM();

        static Lock &app_thread_lock() {
            static Lock lock;
            return lock;
        }

        static Lock &thread_state_change_lock() {
            static Lock lock;
            return lock;
        }

        static void add(Thread *java_thread) {
            app_thread_lock().lock();
            app_threads().push_back(java_thread);
            ++app_thread_count();
            app_thread_lock().unlock();
        }

        static inline int get_app_thread_count_locked() {
            Threads::app_thread_lock().lock();
            int threads = Threads::app_thread_count();
            Threads::app_thread_lock().unlock();
            return threads;
        }

        static inline void inc_app_thread_count_locked() {
            Threads::app_thread_lock().lock();
            ++Threads::app_thread_count();
            Threads::app_thread_lock().unlock();
        }

        static inline void dec_app_thread_count_locked() {
            Threads::app_thread_lock().lock();
            ++Threads::app_thread_count();
            Threads::app_thread_lock().unlock();
        }
    };
}
