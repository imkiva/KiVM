//
// Created by kiva on 2018/4/16.
//
#define _JNI_IMPLEMENTATION_

#include <kivm/kivm.h>

JNI_ENTRY(jint, DestroyJavaVM(JavaVM * vm)) {
    PANIC("jvm_DestroyJavaVM()");
}

JNI_ENTRY(jint, AttachCurrentThread(JavaVM * vm, void * *penv, void * args)) {
    PANIC("jvm_AttachCurrentThread()");
}

JNI_ENTRY(jint, DetachCurrentThread(JavaVM * vm)) {
    PANIC("jvm_DetachCurrentThread()");
}

JNI_ENTRY(jint, GetEnv(JavaVM * vm, void * *penv, jint version)) {
    if (vm == nullptr) {
        return JNI_ERR;
    }
    switch (version) {
        case JNI_VERSION_1_1:
        case JNI_VERSION_1_2:
        case JNI_VERSION_1_4:
        case JNI_VERSION_1_6:
            break;
        default:
            return JNI_ERR;
    }

    auto *env = (JNIEnv *) penv;
    auto nativeInterface = (JNINativeInterface_ *) env->functions;
    kivm::KiVM::fillInterfaceFunctions(nativeInterface);
    return JNI_OK;
}

JNI_ENTRY(jint, AttachCurrentThreadAsDaemon(JavaVM * vm, void * *penv, void * args)) {
    PANIC("jvm_AttachCurrentThreadAsDaemon()");
}

