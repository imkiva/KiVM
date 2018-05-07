//
// Created by kiva on 2018/3/25.
//

#include <kivm/oop/method.h>
#include <kivm/runtime/thread.h>
#include <kivm/runtime/runtimeConfig.h>
#include <kivm/bytecode/interpreter.h>
#include <kivm/oop/primitiveOop.h>
#include <algorithm>
#include <kivm/bytecode/invocationContext.h>

namespace kivm {
    Thread::Thread(Method *method, const std::list<oop> &args)
        : _frames(RuntimeConfig::get().threadMaxStackFrames),
          _method(method), _args(args),
          _javaThreadObject(nullptr), _nativeThread(nullptr),
          _pc(0), _state(ThreadState::RUNNING) {
    }

    void Thread::create(instanceOop javaThread) {
        this->_javaThreadObject = javaThread;
        if (this->shouldRecordInThreadTable()) {
            D("shouldRecordInThreadTable == true, recording.");
            Threads::add(this);
        }

        this->_nativeThread = new std::thread([this] {
            this->start();
        });
        this->onThreadLaunched();
    }

    void Thread::onThreadLaunched() {
        // Do nothing.
    }

    bool Thread::shouldRecordInThreadTable() {
        return true;
    }

    long Thread::getEetop() const {
        return (long) this->_nativeThread->native_handle();
    }

    int Thread::tryHandleException(instanceOop exceptionOop) {
        auto currentMethod = _frames.getCurrentFrame()->getMethod();
        int handler = currentMethod->findExceptionHandler(_pc,
            exceptionOop->getInstanceClass());

        if (handler > 0) {
            return handler;
        }

        this->_exceptionOop = exceptionOop;
        return -1;
    }

    Thread::~Thread() = default;

    JavaThread::JavaThread(Method *method, const std::list<oop> &args)
        : Thread(method, args) {
    }

    void JavaThread::start() {
        // No other threads will join this thread.
        // So it is OK to detach()
        this->_nativeThread->detach();

        // A thread must start with an empty frame
        assert(_frames.getSize() == 0);

        // Only one argument(this) in java.lang.Thread#run()
        assert(_args.size() == 1);

        InvocationContext::invokeWithArgs(this, _method, _args);

        Threads::threadStateChangeLock().lock();
        this->setThreadState(ThreadState::DIED);
        Threads::threadStateChangeLock().unlock();

        if (this->shouldRecordInThreadTable()) {
            Threads::decAppThreadCountLocked();
        }
    }

    Thread *Threads::currentThread() {
        Thread *found = nullptr;
        auto currentThreadID = std::this_thread::get_id();

        Threads::forEachAppThread([&](Thread *thread) {
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
