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
    PANIC("jvm_GetEnv()");
}

JNI_ENTRY(jint, AttachCurrentThreadAsDaemon(JavaVM * vm, void * *penv, void * args)) {
    PANIC("jvm_AttachCurrentThreadAsDaemon()");
}

