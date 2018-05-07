//
// Created by kiva on 2018/3/23.
//
#pragma once

#include <kivm/oop/instanceOop.h>
#include <kivm/runtime/stack.h>
#include <kivm/runtime/frame.h>
#include <list>
#include <thread>
#include <functional>

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
        FrameList _frames;
        std::list<oop> _args;
        u4 _pc;
        
        instanceOop _exceptionOop;

        // note: this is not the current method
        // use getCurrentMethod() instead
        Method *_method;

        virtual void start() = 0;

        virtual bool shouldRecordInThreadTable();

    protected:
        Frame *getCurrentFrame() {
            return _frames.getCurrentFrame();
        }

        void setJavaThreadObject(instanceOop javaThread) {
            this->_javaThreadObject = javaThread;
        }

        int tryHandleException(instanceOop exceptionOop);

    public:
        Thread(Method *method, const std::list<oop> &args);

        virtual ~Thread();

        void create(instanceOop javaThread);

        long getEetop() const;

        virtual void onThreadLaunched();

        inline instanceOop getJavaThreadObject() const {
            return _javaThreadObject;
        }

        inline ThreadState getThreadState() const {
            return _state;
        }

        inline void setThreadState(ThreadState threadState) {
            Thread::_state = threadState;
        }

        inline Method *getCurrentMethod() const {
            if (_frames.getSize() == 0) {
                return nullptr;
            }
            return _frames.getCurrentFrame()->getMethod();
        }

        inline bool isExceptionOccurred() const {
            return _exceptionOop != nullptr;
        }
    };

    // The Java app thread
    class JavaThread : public Thread {
        friend class InvocationContext;

    public:
        JavaThread(Method *method, const std::list<oop> &args);

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

        static std::vector<Thread *> &getAppThreadList() {
            static std::vector<Thread *> appThreads;
            return appThreads;
        }

    public:
        static void initializeJVM(JavaMainThread *thread);

        static Thread *currentThread();

        static void forEachAppThread(const std::function<bool(Thread *)> &callback) {
            LockGuard lockGuard(appThreadLock());
            for (auto it : getAppThreadList()) {
                if (callback(it)) {
                    break;
                }
            }
        }

        static Lock &appThreadLock() {
            static Lock lock;
            return lock;
        }

        static Lock &threadStateChangeLock() {
            static Lock lock;
            return lock;
        }

        static inline void add(Thread *javaThread) {
            D("Adding thread: %p", javaThread);
            LockGuard lockGuard(appThreadLock());
            getAppThreadList().push_back(javaThread);
            ++Threads::getAppThreadCount();
        }

        static inline int getAppThreadCountLocked() {
            LockGuard lockGuard(appThreadLock());
            int threads = Threads::getAppThreadCount();
            return threads;
        }

        static inline void incAppThreadCountLocked() {
            LockGuard lockGuard(appThreadLock());
            ++Threads::getAppThreadCount();
        }

        static inline void decAppThreadCountLocked() {
            LockGuard lockGuard(appThreadLock());
            ++Threads::getAppThreadCount();
        }
    };
}
