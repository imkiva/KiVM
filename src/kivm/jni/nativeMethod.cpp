//
// Created by kiva on 2018/11/13.
//
#include <kivm/jni/nativeMethod.h>
#include <kivm/jni/nativeMethodPool.h>

namespace kivm {
    JavaNativeMethod *JavaNativeMethod::resolve(Method *method) {
        return NativeMethodPool::get()->resolve(method);
    }

    JavaNativeMethod::JavaNativeMethod(Method *method, JNIMethodPointer target)
        : javaMethod(method), invocationTarget(target) {
    }

    JavaNativeMethod::~JavaNativeMethod() = default;
}
