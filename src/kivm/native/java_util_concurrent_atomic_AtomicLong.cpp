//
// Created by kiva on 2018/5/26.
//

#include <compileTimeConfig.h>
#include <kivm/kivm.h>

JAVA_NATIVE jboolean Java_java_util_concurrent_atomic_AtomicLong_VMSupportsCS8(JNIEnv *env, jclass unused) {
#ifdef KIVM_SUPPORT_CS8
    return JNI_TRUE;
#else
    return JNI_FALSE;
#endif
}
