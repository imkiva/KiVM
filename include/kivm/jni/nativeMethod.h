//
// Created by kiva on 2018/11/12.
//
#pragma once

#include <kivm/oop/method.h>

namespace kivm {
    using JNIMethodPointer = void *;

    class NativeMethodPool;

    class JavaNativeMethod {
        friend class NativeMethodPool;

    public:
        static JavaNativeMethod *resolve(Method *method);

    private:
        Method *javaMethod;
        JNIMethodPointer invocationTarget;

    private:
        JavaNativeMethod(Method *method, JNIMethodPointer target);

        ~JavaNativeMethod();

    public:
        inline JNIMethodPointer getInvocationTarget() const {
            return invocationTarget;
        }
    };
}

