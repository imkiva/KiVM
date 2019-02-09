//
// Created by kiva on 2018/5/16.
//

#include <kivm/kivm.h>
#include <kivm/bytecode/execution.h>
#include <kivm/native/classNames.h>

using namespace kivm;

JAVA_NATIVE void Java_java_lang_ClassLoader_registerNatives(JNIEnv *env, jclass java_lang_ClassLoader) {
    D("java/lang/ClassLoader.registerNatives()V");
}

JAVA_NATIVE jclass Java_java_lang_ClassLoader_findLoadedClass0(JNIEnv *env, jclass unused, jstring jname) {
    auto nameObj = Resolver::instance(jname);
    if (nameObj == nullptr) { SHOULD_NOT_REACH_HERE(); }

    const auto &classBinaryName = java::lang::String::toNativeString(nameObj);
    const auto &fixedName = strings::replaceAll(classBinaryName, Global::DOT, Global::SLASH);

    auto dict = SystemDictionary::get();
    auto klass = dict->find(fixedName);
    return klass == nullptr ? nullptr : klass->getJavaMirror();
}
