//
// Created by kiva on 2018/3/25.
//

#include <kivm/oop/method.h>
#include <kivm/runtime/abstractThread.h>
#include <kivm/runtime/runtimeConfig.h>
#include <kivm/oop/primitiveOop.h>
#include <kivm/bytecode/invocationContext.h>
#include <algorithm>

namespace kivm {
    AbstractThread::AbstractThread()
        : _nativeThread(nullptr),
          _state(ThreadState::RUNNING) {
    }

    void AbstractThread::start() {
        this->_nativeThread = new std::thread([this] {
            this->run();
            this->onDestroy();
        });
        this->onThreadLaunched();
    }

    void AbstractThread::onThreadLaunched() {
        // Do nothing.
    }

    void AbstractThread::onDestroy() {
        // do nothing.
    }

    long AbstractThread::getEetop() const {
        return (long) this->_nativeThread->native_handle();
    }

    AbstractThread::~AbstractThread() = default;
}
