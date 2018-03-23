//
// Created by kiva on 2018/3/23.
//
#pragma once

#include <kivm/runtime/stack.h>

namespace kivm {
    class AbstractThread {
    public:
        virtual ~AbstractThread() = default;
    };

    class Thread : public AbstractThread {
    private:
    public:
        ~Thread() override = default;
    };
}
