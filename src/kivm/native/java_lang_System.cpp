//
// Created by kiva on 2018/4/21.
//

#include <kivm/kivm.h>
#include <kivm/native/classNames.h>
#include <kivm/bytecode/execution.h>
#include <kivm/oop/arrayKlass.h>
#include <kivm/oop/arrayOop.h>

using namespace kivm;

JAVA_NATIVE jobject Java_java_lang_System_initProperties(JNIEnv *env, jobject propertiesObject) {
    return propertiesObject;
}

JAVA_NATIVE void Java_java_lang_System_arraycopy(JNIEnv *env, jclass java_lang_System,
                                                 jobject javaSrc, jint srcPos,
                                                 jobject javaDest, jint destPos,
                                                 jint length) {
    auto srcOop = Resolver::resolveArray(javaSrc);
    auto destOop = Resolver::resolveArray(javaDest);

    if (srcOop == nullptr || destOop == nullptr) {
        PANIC("java.lang.NullPointerException");
    }

    auto arrayClass = (ArrayKlass *) srcOop->getClass();
    arrayClass->copyArrayTo(destOop, srcPos, destPos, length);
}
