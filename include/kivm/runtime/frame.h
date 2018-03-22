//
// Created by kiva on 2018/3/23.
//
#pragma once

#include <kivm/runtime/stack.h>

namespace kivm {
    struct Frame {
        Frame *_previous;
        Locals _locals;
        Stack _stack;

        Frame(int max_locals, int max_stacks);
    };
}
