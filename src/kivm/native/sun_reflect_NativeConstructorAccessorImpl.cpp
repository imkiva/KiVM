//
// Created by kiva on 2018/5/24.
//

#include <kivm/kivm.h>
#include <kivm/runtime/thread.h>
#include <kivm/native/classNames.h>
#include <kivm/bytecode/execution.h>
#include <kivm/oop/mirrorOop.h>
#include <kivm/oop/instanceKlass.h>

using namespace kivm;

JAVA_NATIVE jobject Java_sun_reflect_NativeConstructorAccessorImpl_newInstance0(JNIEnv *env, jclass cls,
                                                                                jobject javaConstructor,
                                                                                jobjectArray javaArguments) {
    auto ctorOop = Resolver::instance(javaConstructor);
    auto targetClass = getClassFromConstructor(ctorOop);
    return nullptr;
}
