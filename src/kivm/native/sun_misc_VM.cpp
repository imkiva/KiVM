//
// Created by kiva on 2018/4/24.
//

#include <kivm/kivm.h>

using namespace kivm;

JAVA_NATIVE void Java_sun_misc_VM_initialize(JNIEnv *env, jclass sun_misc_VM) {
    D("native: sun/misc/VM.initialize(): JVM initialized");
}
