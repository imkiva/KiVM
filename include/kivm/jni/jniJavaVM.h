//
// Created by kiva on 2018/4/16.
//
#pragma once

#include <kivm/kivm.h>

JNI_ENTRY(jint, DestroyJavaVM(JavaVM * vm));

JNI_ENTRY(jint, AttachCurrentThread(JavaVM * vm, void * *penv, void * args));

JNI_ENTRY(jint, DetachCurrentThread(JavaVM * vm));

JNI_ENTRY(jint, GetEnv(JavaVM * vm, void * *penv, jint version));

JNI_ENTRY(jint, AttachCurrentThreadAsDaemon(JavaVM * vm, void * *penv, void * args));
