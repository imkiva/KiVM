//
// Created by kiva on 2018/4/21.
//

#include <kivm/kivm.h>
#include <kivm/native/classNames.h>

using namespace kivm;

JAVA_NATIVE jobject Java_java_lang_System_initProperties(JNIEnv *env, jobject propertiesObject) {
    return propertiesObject;
}

JAVA_NATIVE void Java_java_lang_System_arraycopy(JNIEnv *env, jclass java_lang_System,
                                                 jobject javaSrc, jint srcPos,
                                                 jobject JavaDest, jint destPos,
                                                 jint length) {
    PANIC("System.arraycopy() not implemented.");
}
