//
// Created by kiva on 2018/4/21.
//

#include <kivm/kivm.h>
#include <kivm/native/classNames.h>

using namespace kivm;

JAVA_NATIVE void Java_java_lang_Object_registerNatives(JNIEnv *env, jclass java_lang_Object) {
    D("java/lang/Object.registerNatives()V");
}

JAVA_NATIVE jint Java_java_lang_Object_hashCode(JNIEnv *env, jobject javaObject) {
    D("java/lang/Object.hashCode()I");
    return 0;
}

