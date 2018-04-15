//
// Created by kiva on 2018/4/15.
//

#include <kivm/native/java_lang_Thread.h>
#include <kivm/kivm.h>

extern "C" jobject Java_java_lang_Thread_currentThread() {
    PANIC("Native method: java/lang/Thread.currentThread() not implemented");
}

