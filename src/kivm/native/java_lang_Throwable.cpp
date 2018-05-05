//
// Created by kiva on 2018/4/15.
//

#include <kivm/kivm.h>
#include <kivm/bytecode/execution.h>
#include <kivm/native/classNames.h>

using namespace kivm;

JAVA_NATIVE jobject Java_java_lang_Throwable_fillInStackTrace(JNIEnv *env, jobject javaThrowable, jint depth) {
    auto instanceOop = Resolver::resolveInstance(javaThrowable);
    return instanceOop;
}
