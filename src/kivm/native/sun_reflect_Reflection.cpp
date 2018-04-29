//
// Created by kiva on 2018/4/29.
//

#include <kivm/kivm.h>
#include <kivm/runtime/thread.h>
#include <kivm/native/classNames.h>

using namespace kivm;

JAVA_NATIVE jobject Java_sun_reflect_Reflection_getCallerClass(JNIEnv *env, jclass sun_reflect_Reflection) {
    auto currentThread = Threads::currentThread();
    if (currentThread == nullptr) {
        PANIC("currentThread cannot be null");
    }

    // TODO: security stack walk check
    return currentThread->getCurrentMethod()->getClass()->getJavaMirror();
}
