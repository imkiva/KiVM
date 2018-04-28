//
// Created by kiva on 2018/4/28.
//

#include <kivm/kivm.h>
#include <kivm/native/classNames.h>

using namespace kivm;

JAVA_NATIVE void Java_sun_misc_Unsafe_registerNatives(JNIEnv *env, jclass sun_misc_Unsafe) {
    D("sun/misc/Unsafe.registerNatives()V");
}

JAVA_NATIVE jint Java_sun_misc_Unsafe_arrayBaseOffset(JNIEnv *env, jobject javaUnsafe,
                                                      jobject mirror) {
    D("sun/misc/Unsafe.arrayBaseOffset(Ljava/lang/Class;)I");
    return 0;
}

JAVA_NATIVE jint Java_sun_misc_Unsafe_arrayIndexScale(JNIEnv *env, jobject javaUnsafe,
                                                      jobject mirror) {
    D("sun/misc/Unsafe.arrayIndexScale(Ljava/lang/Class;)I");
    return sizeof(intptr_t);
}

JAVA_NATIVE jint Java_sun_misc_Unsafe_addressSize(JNIEnv *env, jobject javaUnsafe) {
    D("sun/misc/Unsafe.addressSize()I");
    return sizeof(intptr_t);
}

