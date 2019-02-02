//
// Created by kiva on 2018/4/30.
//

#include <kivm/kivm.h>
#include <kivm/native/classNames.h>
#include <kivm/bytecode/execution.h>
#include <kivm/bytecode/javaCall.h>

using namespace kivm;

JAVA_NATIVE jobject Java_java_security_AccessController_doPrivileged(JNIEnv *env,
                                                                     jclass java_security_AccessController,
                                                                     jobject javaPrivilegedExceptionAction) {
    auto actionOop = Resolver::instance(javaPrivilegedExceptionAction);
    if (actionOop == nullptr) {
        auto thread = Threads::currentThread();
        assert(thread != nullptr);
        thread->throwException(Global::java_lang_NullPointerException);
        return nullptr;
    }

    // there are many versions of doPrivileged().
    // but we can do the simplest one
    auto actionClass = (InstanceKlass *) actionOop->getClass();
    auto run = actionClass->getVirtualMethod(L"run", L"()Ljava/lang/Object;");
    if (run == nullptr) {
        PANIC("no run() method found");
        return nullptr;
    }

    auto currentThread = Threads::currentThread();
    if (currentThread == nullptr) {
        PANIC("currentThread cannot be null");
    }

    D("native: AccessController.doPrivileged(): performing privileged actions");
    oop result = JavaCall::withArgs(currentThread, run, {actionOop});

    return result;
}

JAVA_NATIVE jobject Java_java_security_AccessController_getStackAccessControlContext(JNIEnv *env,
                                                                                     jclass java_security_AccessController) {
    return nullptr;
}
