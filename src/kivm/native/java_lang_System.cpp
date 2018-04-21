//
// Created by kiva on 2018/4/21.
//

#include <kivm/kivm.h>
#include <kivm/native/classNames.h>

using namespace kivm;

extern "C" jobject Java_java_lang_System_initProperties(JNIEnv *env, jobject propertiesObject) {
    return propertiesObject;
}

