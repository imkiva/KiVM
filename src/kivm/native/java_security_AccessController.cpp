//
// Created by kiva on 2018/4/30.
//

#include <kivm/kivm.h>
#include <kivm/native/classNames.h>
#include <kivm/bytecode/execution.h>
#include <kivm/bytecode/invocationContext.h>

using namespace kivm;

JAVA_NATIVE jobject Java_java_security_AccessController_doPrivileged(JNIEnv *env,
                                                                     jclass java_security_AccessController,
                                                                     jobject javaPrivilegedExceptionAction) {
    auto actionOop = Resolver::resolveInstance(javaPrivilegedExceptionAction);
    if (actionOop == nullptr) {
        PANIC("java.lang.NullPointerException");
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

    auto currentThread = (JavaThread *) Threads::currentThread();
    if (currentThread == nullptr) {
        PANIC("currentThread cannot be null");
    }

    D("native: AccessController.doPrivileged(): performing privileged actions");
    oop result = InvocationContext::invokeWithArgs(currentThread, run, {actionOop});

    // TODO: check whether exception occurred
    return result;
}

JAVA_NATIVE jobject Java_java_security_AccessController_getStackAccessControlContext(JNIEnv *env,
                                                                                     jclass java_security_AccessController) {
    return nullptr;
}
