//
// Created by kiva on 2018/4/21.
//

#include <kivm/kivm.h>
#include <kivm/native/classNames.h>
#include <kivm/bytecode/execution.h>

using namespace kivm;

JAVA_NATIVE void Java_java_lang_Object_registerNatives(JNIEnv *env, jclass java_lang_Object) {
    D("java/lang/Object.registerNatives()V");
}

JAVA_NATIVE jint Java_java_lang_Object_hashCode(JNIEnv *env, jobject javaObject) {
    D("java/lang/Object.hashCode()I");
    auto obj = Resolver::instance(javaObject);
    auto hash = obj->getMarkOop()->getHash();
    if (hash != 0) {
        return hash;
    }

    intptr_t addrBits = intptr_t(javaObject) >> 3;
    intptr_t value = addrBits ^(addrBits >> 5) ^Global::runtimeRandom;
    obj->getMarkOop()->setHash((jint) value);
    return (jint) value;
}

JAVA_NATIVE jobject Java_java_lang_Object_clone(JNIEnv *env, jobject javaObject) {
    static auto java_lang_Cloneable = (InstanceKlass *) BootstrapClassLoader::get()
        ->loadClass(L"java/lang/Cloneable");
    auto thisObject = Resolver::javaOop(javaObject);

    if (thisObject->getClass()->getClassType() == ClassType::INSTANCE_CLASS) {
        auto instance = (instanceOop) thisObject;
        if (instance->getInstanceClass()->checkInterface(java_lang_Cloneable)) {
            return ((instanceOop) thisObject)->shallowCopy();
        }
        PANIC("Clone operation requires class to implement java.lang.Cloneable");
    }

    if (thisObject->getClass()->getClassType() == ClassType::TYPE_ARRAY_CLASS
        || thisObject->getClass()->getClassType() == ClassType::OBJECT_ARRAY_CLASS) {
        PANIC("not supported");
    }

    PANIC("Unknown class type");
}