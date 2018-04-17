//
// Created by kiva on 2018/4/16.
//

#include <kivm/kivm.h>

JNI_ENTRY(jint, DestroyJavaVM(JavaVM * vm)) {
    return kivm::KiVM::destroyJavaVM(vm);
}

JNI_ENTRY(jint, AttachCurrentThread(JavaVM * vm, void * *penv, void * args)) {
    PANIC("jvm_AttachCurrentThread()");
}

JNI_ENTRY(jint, DetachCurrentThread(JavaVM * vm)) {
    PANIC("jvm_DetachCurrentThread()");
}

JNI_ENTRY(jint, GetEnv(JavaVM * vm, void * *penv, jint version)) {
    return kivm::KiVM::getEnv(vm, (JNIEnv **) penv, version);
}

JNI_ENTRY(jint, AttachCurrentThreadAsDaemon(JavaVM * vm, void * *penv, void * args)) {
    PANIC("jvm_AttachCurrentThreadAsDaemon()");
}

