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

    auto currentFrame = currentThread->getCurrentFrame();
    auto previousFrame = currentFrame->getPrevious();
    Frame *found = nullptr;

    D("getCallerClass(): walk %s",
        strings::toStdString(currentFrame->getMethod()->getName()).c_str());

    while (previousFrame != nullptr) {
        D("getCallerClass(): walk %s",
            strings::toStdString(previousFrame->getMethod()->getName()).c_str());
        previousFrame = previousFrame->getPrevious();
    }

    if (found == nullptr) {
        PANIC("getCallerClass(): not previous frame available");
    }

    return found->getMethod()->getClass()->getJavaMirror();
}

JAVA_NATIVE jint Java_sun_reflect_Reflection_getClassAccessFlags(JNIEnv *env,
                                                                 jclass sun_reflect_Reflection,
                                                                 jobject java_lang_Class_mirror) {
    auto classMirror = Resolver::resolveMirror(java_lang_Class_mirror);
    auto mirrorTarget = classMirror->getMirrorTarget();

    if (mirrorTarget == nullptr) {
        return ACC_ABSTRACT | ACC_FINAL | ACC_PUBLIC;
    }

    auto instanceClass = (InstanceKlass *) mirrorTarget;
    return instanceClass->getAccessFlag();
}
