//
// Created by kiva on 2018/2/25.
//
#pragma once

#include <kivm/kivm.h>
#include <kivm/oop/oop.h>
#include <kivm/runtime/thread.h>

namespace kivm {
    class ByteCodeInterpreter {
    public:
        static oop interp(JavaThread *thread);
    };
}
