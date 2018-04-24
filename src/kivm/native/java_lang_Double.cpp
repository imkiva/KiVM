//
// Created by kiva on 2018/4/24.
//

#include <kivm/kivm.h>
#include <kivm/native/classNames.h>

using namespace kivm;

JAVA_NATIVE jlong Java_java_lang_Double_doubleToRawLongBits(JNIEnv *env, jclass java_lang_Double, jdouble d) {
    union {
        jlong j;
        jdouble d;
    } u{};
    u.d = d;
    return u.j;
}

JAVA_NATIVE jdouble Java_java_lang_Double_longBitsToDouble(JNIEnv *env, jclass java_lang_Double, jlong j) {
    union {
        jlong j;
        jdouble d;
    } u{};
    u.j = j;
    return u.d;
}

