//
// Created by kiva on 2018/4/24.
//

#include <kivm/kivm.h>
#include <kivm/native/classNames.h>

using namespace kivm;

extern "C" jint Java_java_lang_Float_floatToRawIntBits(JNIEnv *env, jclass java_lang_Float, jfloat f) {
    union {
        jint i;
        jfloat f;
    } u{};
    u.f = f;
    return u.i;
}
