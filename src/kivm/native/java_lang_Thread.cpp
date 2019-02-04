//
// Created by kiva on 2018/4/15.
//

#include <kivm/native/java_lang_Thread.h>
#include <kivm/kivm.h>
#include <kivm/bytecode/execution.h>
#include <kivm/native/classNames.h>
#include <kivm/oop/primitiveOop.h>

#include <pthread.h>
#include <csignal>

using namespace kivm;

JAVA_NATIVE void Java_java_lang_Thread_registerNatives(JNIEnv *env, jclass java_lang_Thread) {
    D("java/lang/Thread.registerNatives()V");
}

JAVA_NATIVE jobject Java_java_lang_Thread_currentThread(JNIEnv *env, jclass java_lang_Thread) {
    auto currentThread = Threads::currentThread();
    if (currentThread == nullptr) {
        PANIC("currentThread cannot be null");
    }

    return currentThread->getJavaThreadObject();
}

JAVA_NATIVE void Java_java_lang_Thread_setPriority0(JNIEnv *env, jobject threadObject, jint priority) {
    auto instanceOop = Resolver::instance(threadObject);
    auto thread = Threads::searchNativeThread(instanceOop);
    if (thread == nullptr) {
        auto current = Threads::currentThread();
        if (current == nullptr) {
            SHOULD_NOT_REACH_HERE();
        }
        current->throwException(Global::_NullPointerException, false);
        return;
    }

    // set native thread's priority
    thread->setPriority(priority);
}

JAVA_NATIVE jboolean Java_java_lang_Thread_isAlive(JNIEnv *env, jobject threadObject) {
    auto threadOop = Resolver::instance(threadObject);
    longOop eetopOop = nullptr;
    if (!threadOop->getFieldValue(J_THREAD, L"eetop", L"J", (oop *) &eetopOop)) {
        SHOULD_NOT_REACH_HERE();
    }

    if (eetopOop == nullptr) {
        return JNI_FALSE;
    }

    long eetop = eetopOop->getValue();
    int ret = pthread_kill((pthread_t) eetop, 0);
    if (ret == 0) {
        return JNI_TRUE;
    } else if (ret == ESRCH) {
        return JNI_FALSE;
    } else {
        SHOULD_NOT_REACH_HERE();
    }
}

JAVA_NATIVE void Java_java_lang_Thread_start0(JNIEnv *env, jobject threadObject) {
    auto threadOop = Resolver::instance(threadObject);
    auto klass = threadOop->getInstanceClass();

    if (klass->getName() == L"java/lang/ref/Reference$ReferenceHandler") {
        return;
    }

    auto runMethod = klass->getVirtualMethod(L"run", L"()V");
    assert(runMethod != nullptr);
    auto thread = new JavaThread(runMethod, {threadOop});
    thread->start(threadOop);
}
