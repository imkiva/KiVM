//
// Created by kiva on 2018/5/5.
//

#include <kivm/kivm.h>
#include <kivm/bytecode/execution.h>
#include <kivm/native/classNames.h>

using namespace kivm;

// TODO: Stack trace support

JAVA_NATIVE jobject Java_java_lang_Throwable_fillInStackTrace(JNIEnv *env, jobject javaThrowable, jint depth) {
    auto instanceOop = Resolver::instance(javaThrowable);
    return instanceOop;
}

JAVA_NATIVE jint Java_java_lang_Throwable_getStackTraceDepth(JNIEnv *env, jobject javaThrowable) {
    return 0;
}

JAVA_NATIVE jobject Java_java_lang_Throwable_getStackTraceElement(JNIEnv *env, jobject javaThrowable, jint index) {
    return nullptr;
}
