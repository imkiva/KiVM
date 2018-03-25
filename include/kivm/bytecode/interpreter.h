//
// Created by kiva on 2018/2/25.
//
#pragma once

#include <kivm/kivm.h>
#include <kivm/oop/oop.h>
#include <kivm/runtime/thread.h>

#define T_BOOLEAN               4
#define T_CHAR                  5
#define T_FLOAT                 6
#define T_DOUBLE                7
#define T_BYTE                  8
#define T_SHORT                 9
#define T_INT                  10
#define T_LONG                 11

namespace kivm {
    class ByteCodeInterpreter {
    public:
        static oop interp(JavaThread *thread);
    };
}
