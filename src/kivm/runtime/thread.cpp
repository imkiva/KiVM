//
// Created by kiva on 2018/3/25.
//

#include <kivm/oop/method.h>
#include <kivm/runtime/thread.h>
#include <kivm/runtime/runtimeConfig.h>
#include <kivm/bytecode/interpreter.h>
#include <kivm/oop/primitiveOop.h>
#include <kivm/bytecode/invocationContext.h>
#include <algorithm>

namespace kivm {
    Thread::Thread()
        : _javaThreadObject(nullptr), _nativeThread(nullptr),
          _state(ThreadState::RUNNING) {
    }

    void Thread::create(instanceOop javaThread) {
        this->_javaThreadObject = javaThread;

        this->_nativeThread = new std::thread([this] {
            this->start();
        });
        this->onThreadLaunched();
    }

    void Thread::onThreadLaunched() {
        // Do nothing.
    }

    long Thread::getEetop() const {
        return (long) this->_nativeThread->native_handle();
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

    Thread::~Thread() = default;

    JavaThread::JavaThread(Method *method, const std::list<oop> &args)
        : _frames(RuntimeConfig::get().threadMaxStackFrames),
          _method(method), _args(args), _pc(0) {
    }

    void JavaThread::create(instanceOop javaThread) {
        Threads::addJavaThread(this);
        Thread::create(javaThread);
    }

    void JavaThread::start() {
        // A thread must start with an empty frame
        assert(_frames.getSize() == 0);

        // Only one argument(this) in java.lang.Thread#run()
        assert(_args.size() == 1);

        InvocationContext::invokeWithArgs(this, _method, _args);

        Threads::threadStateChangeLock().lock();
        this->setThreadState(ThreadState::DIED);
        Threads::threadStateChangeLock().unlock();

        // do not remove thread instance in thread list
        // just tell thread list how many active thread are still running
        Threads::notifyJavaThreadDeadLocked(this);
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
