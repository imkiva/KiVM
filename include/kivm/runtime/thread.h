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

        friend class InvocationContext;

        friend class KiVM;

    protected:
        instanceOop _javaThreadObject;
        std::thread *_nativeThread;

        ThreadState _state;

        virtual void start() = 0;

        virtual void destroy();

    protected:
        void setJavaThreadObject(instanceOop javaThread) {
            this->_javaThreadObject = javaThread;
        }

    public:
        Thread();

        virtual ~Thread();

        virtual void create(instanceOop javaThread);

        virtual void onThreadLaunched();

        long getEetop() const;

        inline instanceOop getJavaThreadObject() const {
            return _javaThreadObject;
        }

        inline ThreadState getThreadState() const {
            return _state;
        }

        inline void setThreadState(ThreadState threadState) {
            Thread::_state = threadState;
        }
    };

    // The Java app thread
    class JavaThread : public Thread {
        friend class Threads;

        friend class ByteCodeInterpreter;

        friend class InvocationContext;

        friend class KiVM;

        friend class InvocationContext;

    protected:
        FrameList _frames;
        std::list<oop> _args;
        u4 _pc;

        instanceOop _exceptionOop;

        // note: this is not the current method
        // use getCurrentMethod() instead
        Method *_method;

    protected:
        void start() override;

        int tryHandleException(instanceOop exceptionOop);

    public:
        JavaThread(Method *method, const std::list<oop> &args);

        void create(instanceOop javaThread) override;

        void destroy() override;

        inline Frame *getCurrentFrame() {
            return _frames.getCurrentFrame();
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

    // The Java main thread
    // implemented in src/kivm/runtime/init.cpp
    class JavaMainThread : public JavaThread {
    private:
        String _mainClassName;
        std::vector<String> _arguments;

    public:
        JavaMainThread(const String &mainClassName, const std::vector<String> &arguments);

    protected:
        void start() override;

        void onThreadLaunched() override;
    };

    class Threads {
    private:
        static int &getRunningJavaThreadCount() {
            static int appThreadCount;
            return appThreadCount;
        }

        static std::vector<JavaThread *> &getJavaThreadList() {
            static std::vector<JavaThread *> appThreads;
            return appThreads;
        }

        static void initializeVMStructs(BootstrapClassLoader *cl, JavaMainThread *thread);

        static void hackJavaClasses(BootstrapClassLoader *cl, JavaMainThread *thread);

    public:
        static void initializeJVM(JavaMainThread *thread);

        static JavaThread *currentThread();

        static void forEach(const std::function<bool(JavaThread *)> &callback) {
            LockGuard lockGuard(appThreadLock());
            for (auto it : getJavaThreadList()) {
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

        static inline void addJavaThread(JavaThread *javaThread) {
            LockGuard lockGuard(appThreadLock());
            D("Adding thread: %p", javaThread);
            getJavaThreadList().push_back(javaThread);
            ++Threads::getRunningJavaThreadCount();
        }

        static inline int getRunningJavaThreadCountLocked() {
            LockGuard lockGuard(appThreadLock());
            int threads = Threads::getRunningJavaThreadCount();
            return threads;
        }

        static inline void notifyJavaThreadDeadLocked(JavaThread *javaThread) {
            LockGuard lockGuard(appThreadLock());
            --Threads::getRunningJavaThreadCount();
        }
    };
}
