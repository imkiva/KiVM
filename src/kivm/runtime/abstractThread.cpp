//
// Created by kiva on 2018/3/25.
//

#include <compileTimeConfig.h>
#include <kivm/runtime/abstractThread.h>
#include <kivm/oop/method.h>
#include <kivm/oop/primitiveOop.h>
#include <kivm/runtime/runtimeConfig.h>
#include <kivm/bytecode/invocationContext.h>
#include <algorithm>

#ifdef KIVM_PLATFORM_UNIX
#include <pthread.h>
#endif

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

    long AbstractThread::getNativeHandler() const {
        return (long) this->_nativeThread->native_handle();
    }

    void AbstractThread::setThreadName(const String &name) {
#if defined(KIVM_PLATFORM_APPLE)
        pthread_setname_np(strings::toStdString(name).c_str());
#elif defined(KIVM_PLATFORM_UNIX)
        pthread_setname_np(pthread_self(), strings::toStdString(name).c_str());
#endif
    }

    AbstractThread::~AbstractThread() = default;
}
