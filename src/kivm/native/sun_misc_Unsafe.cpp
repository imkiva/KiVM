//
// Created by kiva on 2018/4/28.
//

#include <kivm/kivm.h>
#include <kivm/native/classNames.h>

using namespace kivm;

JAVA_NATIVE void Java_sun_misc_Unsafe_registerNatives(JNIEnv *env, jclass sun_misc_Unsafe) {
    D("sun/misc/Unsafe.registerNatives()V");
}

JAVA_NATIVE jint Java_sun_misc_Unsafe_arrayBaseOffset(JNIEnv *env, jclass sun_misc_Unsafe) {
    D("sun/misc/Unsafe.arrayBaseOffset(Ljava/lang/Class;)I");
    return 0;
}

