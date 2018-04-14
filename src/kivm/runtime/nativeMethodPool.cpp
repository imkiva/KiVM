//
// Created by kiva on 2018/4/14.
//
#include <kivm/runtime/nativeMethodPool.h>

namespace kivm {
    void *NativeMethodPool::resolve(Method *method) {
        return nullptr;
    }

    NativeMethodPool *NativeMethodPool::get() {
        static NativeMethodPool pool;
        return &pool;
    }
}
