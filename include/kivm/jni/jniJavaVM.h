//
// Created by kiva on 2018/4/16.
//
#pragma once

#include <kivm/kivm.h>

JVM_ENTRY(jint, DestroyJavaVM(JavaVM * vm));

JVM_ENTRY(jint, AttachCurrentThread(JavaVM * vm, void * *penv, void * args));

JVM_ENTRY(jint, DetachCurrentThread(JavaVM * vm));

JVM_ENTRY(jint, GetEnv(JavaVM * vm, void * *penv, jint version));

JVM_ENTRY(jint, AttachCurrentThreadAsDaemon(JavaVM * vm, void * *penv, void * args));
