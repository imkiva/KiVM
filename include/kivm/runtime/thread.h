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
        friend class Threads;
        friend class ByteCodeInterpreter;

    protected:
        instanceOop _java_thread_object;
        std::thread *_native_thread;

        ThreadState _state;
        Method *_method;
        FrameList _frames;
        std::list<oop> _args;
        u4 _pc;

        virtual void start() = 0;

        virtual bool shouldRecordInThreadTable();

    protected:
        Frame * getCurrentFrame() {
            return _frames.getCurrentFrame();
        }

        void setJavaThreadObject(instanceOop java_thread) {
            this->_java_thread_object = java_thread;
        }

    public:
        Thread(Method *method, const std::list<oop> &args);

        virtual ~Thread();

        void create(instanceOop java_thread);

        long getEetop() const;

        virtual void onThreadLaunched();

        inline instanceOop getJavaThreadObject() const {
            return _java_thread_object;
        }

        ThreadState getThreadState() const {
            return _state;
        }

        void setThreadState(ThreadState _state) {
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

        oop runMethod(Method *method, const std::list<oop> &args);
    };

    // The Java main thread
    // implemented in src/kivm/runtime/init.cpp
    class JavaMainThread : public JavaThread {
    public:
        JavaMainThread();

    protected:
        void start() override;

        void onThreadLaunched() override;

        bool shouldRecordInThreadTable() override;
    };

    class Threads {
    private:
        static int &getAppThreadCount() {
            static int app_thread_count;
            return app_thread_count;
        }

        static std::list<Thread *> getAppThreadList() {
            static std::list<Thread *> app_threads;
            return app_threads;
        }

    public:
        static void initializeJVM(JavaMainThread *thread);

        static Lock &appThreadLock() {
            static Lock lock;
            return lock;
        }

        static Lock &threadStateChangeLock() {
            static Lock lock;
            return lock;
        }

        static void add(Thread *java_thread) {
            appThreadLock().lock();
            getAppThreadList().push_back(java_thread);
            ++getAppThreadCount();
            appThreadLock().unlock();
        }

        static inline int getAppThreadCountLocked() {
            Threads::appThreadLock().lock();
            int threads = Threads::getAppThreadCount();
            Threads::appThreadLock().unlock();
            return threads;
        }

        static inline void incAppThreadCountLocked() {
            Threads::appThreadLock().lock();
            ++Threads::getAppThreadCount();
            Threads::appThreadLock().unlock();
        }

        static inline void decAppThreadCountLocked() {
            Threads::appThreadLock().lock();
            ++Threads::getAppThreadCount();
            Threads::appThreadLock().unlock();
        }
    };
}
