//
// Created by kiva on 2018/5/30.
//

#include <kivm/oop/method.h>
#include <kivm/runtime/javaThread.h>
#include <kivm/runtime/runtimeConfig.h>
#include <kivm/oop/primitiveOop.h>
#include <kivm/bytecode/invocationContext.h>
#include <kivm/gc/gcThread.h>

namespace kivm {
    JavaThread::JavaThread(Method *method, const std::list<oop> &args)
        : _javaThreadObject(nullptr),
          _exceptionOop(nullptr),
          _frames(RuntimeConfig::get().threadMaxStackFrames),
          _method(method), _args(args), _pc(0),
          _inSafepoint(false) {
    }

    int JavaThread::tryHandleException(instanceOop exceptionOop) {
        this->_exceptionOop = exceptionOop;

        auto currentMethod = _frames.getCurrentFrame()->getMethod();
        int handler = currentMethod->findExceptionHandler(_pc,
            exceptionOop->getInstanceClass());

        if (handler > 0) {
            this->_exceptionOop = nullptr;
            return handler;
        }

        return -1;
    }

    void JavaThread::start(instanceOop javaThread) {
        this->_javaThreadObject = javaThread;
        Threads::addJavaThread(this);
        AbstractThread::start();
    }

    void JavaThread::start() {
        PANIC("java thread object required");
    }

    void JavaThread::run() {
        // No other threads will join this thread.
        // So it is OK to detach()
        this->_nativeThread->detach();

        // A thread must start with an empty frame
        assert(_frames.getSize() == 0);

        // Only one argument(this) in java.lang.Thread#run()
        assert(_args.size() == 1);

        InvocationContext::invokeWithArgs(this, _method, _args);
    }

    void JavaThread::onDestroy() {
        AbstractThread::onDestroy();

        // update java thread status
        Threads::setThreadStateLocked(this, ThreadState::DIED);

        // do not remove thread instance in thread list
        // just tell thread list how many active thread are still running
        Threads::notifyJavaThreadDeadLocked(this);
    }

    void JavaThread::throwException(InstanceKlass *exceptionClass) {
        assert(exceptionClass != nullptr);
        auto ctor = exceptionClass->getThisClassMethod("<init>", "()V");
        auto exceptionOop = exceptionClass->newInstance();
        InvocationContext::invokeWithArgs(this, ctor,
            {exceptionOop},
            true);
        this->_exceptionOop = exceptionOop;
    }

    void JavaThread::throwException(InstanceKlass *exceptionClass, const String &message) {
        assert(exceptionClass != nullptr);
        auto ctor = exceptionClass->getThisClassMethod("<init>", "(Ljava/lang/String;)V");
        auto exceptionOop = exceptionClass->newInstance();
        InvocationContext::invokeWithArgs(this, ctor,
            {exceptionOop, java::lang::String::from(message)},
            true);
        this->_exceptionOop = exceptionOop;
    }

    void JavaThread::enterSafepoint() {
        ThreadState originalState = getThreadState();
        Threads::setThreadStateLocked(this, ThreadState::BLOCKED);
        _inSafepoint = true;
        GCThread::get()->wait();
        _inSafepoint = false;
        Threads::setThreadStateLocked(this, originalState);
    }

    void JavaThread::enterSafepointIfNeeded() {
        if (GCThread::get()->getState() == GCState::WAITING_FOR_SAFEPOINT) {
            enterSafepoint();
        }
    }

    JavaThread *Threads::currentThread() {
        JavaThread *found = nullptr;
        auto currentThreadID = std::this_thread::get_id();

        Threads::forEach([&](JavaThread *thread) {
            if (thread->getThreadState() != ThreadState::DIED) {
                auto checkThreadID = thread->_nativeThread->get_id();
                if (checkThreadID == currentThreadID) {
                    found = thread;
                    return true;
                }
            }
            return false;
        });
        return found;
    }
}
