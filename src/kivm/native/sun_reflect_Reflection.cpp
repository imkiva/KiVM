//
// Created by kiva on 2018/4/29.
//

#include <kivm/kivm.h>
#include <kivm/runtime/thread.h>
#include <kivm/native/classNames.h>
#include <kivm/bytecode/execution.h>
#include <kivm/oop/mirrorOop.h>
#include <kivm/oop/instanceKlass.h>

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

JAVA_NATIVE jint Java_sun_reflect_Reflection_getClassAccessFlags(JNIEnv *env,
                                                                 jclass sun_reflect_Reflection,
                                                                 jobject java_lang_Class_mirror) {
    auto classMirror = Resolver::resolveMirror(java_lang_Class_mirror);
    auto mirrorTarget = classMirror->getMirrorTarget();

    if (mirrorTarget->getClassType() != ClassType::INSTANCE_CLASS) {
        PANIC("native: attempt to get fields of non-instance oops");
    }

    auto instanceClass = (InstanceKlass *) mirrorTarget;
    return instanceClass->getAccessFlag();
}
