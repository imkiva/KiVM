//
// Created by kiva on 2018/2/25.
// Modified by mikecovlee on 2018/2/25.
//

#pragma once

#include <shared/lock.h>
#include <condition_variable>
#include <thread>
#include <mutex>
#include <chrono>

namespace kivm {
    /**
     * StackOverflow:
     *
     * In the case of recursive mutex,
     * you do have to ensure that the given thread has only locked the recursive mutex once,
     * since the condition variable only will use the unlock method
     * on the unique_lock once during the wait.
     */

    class Monitor final {
    private:
        RecursiveLock _mutexLock;
        std::unique_lock<RecursiveLock> _lock;
        std::condition_variable_any _cond;

    public:
        Monitor() : _lock(_mutexLock, std::defer_lock) {
        }

        Monitor(const Monitor &) = delete;

        Monitor(Monitor &&) noexcept = delete;

        ~Monitor() = default;

        void enter() {
            _lock.lock();
        }

        void wait() {
            _cond.wait(_lock);
        }

        void wait(long millisecond) {
            _cond.wait_for(_lock, std::chrono::milliseconds(millisecond));
        }

        void notify() {
            _cond.notify_one();
        }

        void notify_all() {
            _cond.notify_all();
        }

        void leave() {
            _lock.unlock();
        }

        void force_unlock_when_athrow() {
            _lock.try_lock();
            _lock.unlock();
        }
    };
}