//
// Created by kiva on 2018/3/25.
//

#include <kivm/oop/method.h>
#include <kivm/runtime/thread.h>
#include <kivm/runtime/runtimeConfig.h>
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
            this->destroy();
        });
        this->onThreadLaunched();
    }

    void Thread::onThreadLaunched() {
        // Do nothing.
    }

    void Thread::destroy() {
        // do nothing.
    }

    long Thread::getEetop() const {
        return (long) this->_nativeThread->native_handle();
    }

    Thread::~Thread() = default;

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
