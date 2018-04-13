//
// Created by kiva on 2018/2/25.
// Modified by mikecovlee on 2018/2/25.
//

#pragma once

#include <condition_variable>
#include <thread>
#include <mutex>
#include <chrono>

namespace kivm {
    class Monitor final {
    private:
        std::mutex _mutex;
        std::condition_variable _cond;
        std::unique_lock<std::mutex> _lock;

    public:
        Monitor() : _lock(_mutex, std::defer_lock) {
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