//
// Created by kiva on 2018/4/16.
//
#define _JNI_IMPLEMENTATION_

#include <kivm/kivm.h>

JVM_ENTRY(jint, DestroyJavaVM(JavaVM * vm)) {
    PANIC("jvm_DestroyJavaVM()");
}

JVM_ENTRY(jint, AttachCurrentThread(JavaVM * vm, void * *penv, void * args)) {
    PANIC("jvm_AttachCurrentThread()");
}

JVM_ENTRY(jint, DetachCurrentThread(JavaVM * vm)) {
    PANIC("jvm_DetachCurrentThread()");
}

JVM_ENTRY(jint, GetEnv(JavaVM * vm, void * *penv, jint version)) {
    PANIC("jvm_GetEnv()");
}

JVM_ENTRY(jint, AttachCurrentThreadAsDaemon(JavaVM * vm, void * *penv, void * args)) {
    PANIC("jvm_AttachCurrentThreadAsDaemon()");
}

