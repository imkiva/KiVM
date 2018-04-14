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
        instanceOop _javaThreadObject;
        std::thread *_nativeThread;

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

        void setJavaThreadObject(instanceOop javaThread) {
            this->_javaThreadObject = javaThread;
        }

    public:
        Thread(Method *method, const std::list<oop> &args);

        virtual ~Thread();

        void create(instanceOop javaThread);

        long getEetop() const;

        virtual void onThreadLaunched();

        inline instanceOop getJavaThreadObject() const {
            return _javaThreadObject;
        }

        ThreadState getThreadState() const {
            return _state;
        }

        void setThreadState(ThreadState threadState) {
            Thread::_state = threadState;
        }
    };

    // The Java app thread
    class JavaThread : public Thread {
        friend class Execution;

    public:
        JavaThread(Method *method, const std::list<oop> &args);

        oop runMethod(Method *method, const std::list<oop> &args);

    protected:
        void start() override;
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
            static int appThreadCount;
            return appThreadCount;
        }

        static std::list<Thread *> getAppThreadList() {
            static std::list<Thread *> appThreads;
            return appThreads;
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

        static void add(Thread *javaThread) {
            appThreadLock().lock();
            getAppThreadList().push_back(javaThread);
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
