//
// Created by kiva on 2019-02-04.
//

#include <kivm/kivm.h>
#include <kivm/native/classNames.h>

JAVA_NATIVE void Java_java_lang_invoke_MethodHandleNatives_registerNatives(JNIEnv *env, jclass mhn) {
    D("java/lang/MethodHandleNatives.registerNatives()V");
}

JAVA_NATIVE jint Java_java_lang_invoke_MethodHandleNatives_getConstant(JNIEnv *env, jclass mhn, jint i) {
    // Copied from hotspot vm
    assert(i == 4);
    return 0;
}

JAVA_NATIVE jobject Java_java_lang_invoke_MethodHandleNatives_resolve(JNIEnv *env, jclass mhn,
                                                                      jobject memberName, jclass caller) {
    // TODO: implement
    WARN("MethodHandleNatives.resolve() is not implemented");
    return nullptr;
}


