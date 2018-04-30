//
// Created by kiva on 2018/4/30.
//

#include <kivm/kivm.h>
#include <kivm/native/classNames.h>
#include <kivm/bytecode/execution.h>

using namespace kivm;

JAVA_NATIVE jobject Java_java_security_AccessController_doPrivileged(JNIEnv *env,
                                                                     jclass java_security_AccessController,
                                                                     jobject javaPrivilegedExceptionAction) {
    auto actionOop = Resolver::resolveInstance(javaPrivilegedExceptionAction);

}
