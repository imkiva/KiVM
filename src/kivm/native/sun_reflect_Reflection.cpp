//
// Created by kiva on 2018/4/29.
//

#include <kivm/kivm.h>
#include <kivm/runtime/abstractThread.h>
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

    if (currentFrame->getMethod()->getName() != L"getCallerClass") {
        PANIC("getCallerClass(): current method should be getCallerClass()");
    }

    while (previousFrame != nullptr) {
        D("getCallerClass(): walk %s",
            strings::toStdString(previousFrame->getMethod()->getName()).c_str());

        // ignore some special methods and annotations (sun/reflect/CallerSensitive, etc.)
        if (previousFrame->getMethod()->checkAnnotation(L"Lsun/reflect/CallerSensitive;")) {
            previousFrame = previousFrame->getPrevious();
            continue;
        }

        found = previousFrame;
        break;
    }

    if (found == nullptr) {
        PANIC("getCallerClass(): not previous frame available");
    }

    D("getCallerClass(): found caller class: %s",
        strings::toStdString(found->getMethod()->getClass()->getName()).c_str());

    return found->getMethod()->getClass()->getJavaMirror();
}

JAVA_NATIVE jint Java_java_lang_Class_getModifiers(JNIEnv *env, jobject mirror);

JAVA_NATIVE jint Java_sun_reflect_Reflection_getClassAccessFlags(JNIEnv *env,
                                                                 jclass sun_reflect_Reflection,
                                                                 jobject java_lang_Class_mirror) {
    return Java_java_lang_Class_getModifiers(env, java_lang_Class_mirror);
}
