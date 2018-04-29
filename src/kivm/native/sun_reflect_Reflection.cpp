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

    auto method = currentThread->getCurrentMethod();
    if (method == nullptr) {
        // we have walked through the calling stack
        PANIC("wtf");
    }

    // TODO: security stack walk check
    return method->getClass()->getJavaMirror();
}
