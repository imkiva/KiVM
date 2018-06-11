//
// Created by kiva on 2018/4/14.
//
#pragma once

#include <kivm/oop/method.h>
#include <shared/hashMap.h>

namespace kivm {
    class NativeMethodPool {
    private:
        HashMap<Method *, void *> _nativeMethods;

    public:
        static NativeMethodPool *get();

        void set(Method *method, void *nativePointer);

        void *resolve(Method *method);
    };
}
