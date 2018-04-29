//
// Created by kiva on 2018/4/30.
//

#include <kivm/kivm.h>
#include <kivm/native/classNames.h>

using namespace kivm;

JAVA_NATIVE jobject Java_java_security_AccessController_doPrivileged(JNIEnv *env,
                                                                     jclass java_security_AccessController,
                                                                     jobject javaPrivilegedExceptionAction) {
    D("java/security/AccessController.doPrivileged:(Ljava/security/PrivilegedExceptionAction;)Ljava/lang/Object;");
    PANIC("I felt sorry to turn your requests down because I don't know what you want to do.");
}
