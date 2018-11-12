//
// Created by kiva on 2018/4/14.
//
#pragma once

#include <kivm/oop/method.h>
#include <shared/hashMap.h>

namespace kivm {
    class JavaNativeMethod;

    class NativeMethodPool {
    private:
        HashMap<Method *, JavaNativeMethod *> _nativeMethods;

    public:
        static NativeMethodPool *get();

        void set(Method *method, JavaNativeMethod *nativeMethod);

        JavaNativeMethod *resolve(Method *method);
    };
}
