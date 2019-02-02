//
// Created by kiva on 2018/11/12.
//
#pragma once

#include <kivm/oop/method.h>

namespace kivm {
    using JNIMethodPointer = void *;

    class NativeMethodPool;

    class JavaNativeMethod final {
        friend class NativeMethodPool;

    public:
        static JavaNativeMethod *resolve(Method *method);

    private:
        Method *_javaMethod = nullptr;
        JNIMethodPointer _invocationTarget = nullptr;

    private:
        JavaNativeMethod(Method *method, JNIMethodPointer target);

        ~JavaNativeMethod();

    public:
        inline JNIMethodPointer getInvocationTarget() const {
            return _invocationTarget;
        }
    };
}

