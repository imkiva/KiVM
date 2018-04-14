//
// Created by kiva on 2018/4/14.
//
#pragma once

#include <kivm/method.h>

namespace kivm {
    class NativeMethodPool {
    public:
        static void *resolve(Method *method);
    };
}
