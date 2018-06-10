//
// Created by kiva on 2018/4/21.
//

#include <kivm/kivm.h>
#include <kivm/native/classNames.h>
#include <kivm/bytecode/execution.h>
#include <kivm/oop/arrayOop.h>

using namespace kivm;

JAVA_NATIVE void Java_java_lang_Object_registerNatives(JNIEnv *env, jclass java_lang_Object) {
    D("java/lang/Object.registerNatives()V");
}

JAVA_NATIVE jint Java_java_lang_Object_hashCode(JNIEnv *env, jobject javaObject) {
    D("java/lang/Object.hashCode()I");
    auto obj = Resolver::javaOop(javaObject);
    auto hash = obj->getMarkOop()->getHash();
    if (hash != 0) {
        return hash;
    }

    auto value = intptr_t(javaObject);
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
            return ((instanceOop) thisObject)->copy();
        }
        PANIC("Clone operation requires class to implement java.lang.Cloneable");
    }

    if (thisObject->getClass()->getClassType() == ClassType::TYPE_ARRAY_CLASS
        || thisObject->getClass()->getClassType() == ClassType::OBJECT_ARRAY_CLASS) {
        auto array = (arrayOop) thisObject;
        return array->copy();
    }

    PANIC("Unknown class type");
}

JAVA_NATIVE jclass Java_java_lang_Object_getClass(JNIEnv *env, jobject javaObject) {
    auto object = Resolver::javaOop(javaObject);
    return object->getClass()->getJavaMirror();
}

JAVA_NATIVE void Java_java_lang_Object_notifyAll(JNIEnv *env, jobject javaObject) {
    auto object = Resolver::javaOop(javaObject);
    object->getMarkOop()->notifyAll();
}

JAVA_NATIVE void Java_java_lang_Object_notify(JNIEnv *env, jobject javaObject) {
    auto object = Resolver::javaOop(javaObject);
    object->getMarkOop()->notify();
}

JAVA_NATIVE void Java_java_lang_Object_wait(JNIEnv *env, jobject javaObject, jlong timeout) {
    auto object = Resolver::javaOop(javaObject);
    if (timeout == 0) {
        object->getMarkOop()->wait();
    } else {
        object->getMarkOop()->wait(timeout);
    }
}
