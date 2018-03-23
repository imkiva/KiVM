//
// Created by kiva on 2018/3/23.
//
#pragma once

#include <kivm/oop/instanceOop.h>
#include <kivm/runtime/stack.h>
#include <unordered_map>
#include <pthread.h>

namespace kivm {
    class AbstractThread {
    private:
        instanceOop _java_thread{};

    public:
        virtual ~AbstractThread() = default;

        inline instanceOop get_java_thread() const {
            return _java_thread;
        }
    };

    class ThreadTable {
    public:
    };

    class Thread : public AbstractThread {
    private:
    public:
        ~Thread() override = default;
    };
}
