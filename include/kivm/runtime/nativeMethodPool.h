//
// Created by kiva on 2018/4/14.
//
#pragma once

#include <kivm/oop/method.h>
#include <unordered_map>

namespace kivm {
    class NativeMethodPool {
    private:
        std::unordered_map<Method *, void *> _nativeMethods;

    public:
        static NativeMethodPool *get();

        void *resolve(Method *method);
    };
}
