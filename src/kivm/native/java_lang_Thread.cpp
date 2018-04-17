//
// Created by kiva on 2018/4/15.
//

#include <kivm/native/java_lang_Thread.h>
#include <kivm/kivm.h>
#include <kivm/bytecode/execution.h>

extern "C" jobject Java_java_lang_Thread_currentThread(JNIEnv *env, jclass java_lang_Thread) {
    auto instanceKlass = kivm::Resolver::resolveJClass(java_lang_Thread);
    D("JNIEnv: %p, I am %s",
      env,
      kivm::strings::toStdString(instanceKlass->getName()).c_str());

    PANIC("Native method: java/lang/Thread.currentThread() not implemented");
}

