//
// Created by kiva on 2018/4/16.
//
#include <kivm/kivm.h>

_JNI_IMPORT_OR_EXPORT_
jint JNICALL JNI_GetDefaultJavaVMInitArgs(void *args) {
    PANIC("JNI_GetDefaultJavaVMInitArgs()");
}

_JNI_IMPORT_OR_EXPORT_
jint JNICALL JNI_CreateJavaVM(JavaVM **pvm, void **penv, void *args) {
    return kivm::KiVM::createVirtualMachine(pvm);
}

_JNI_IMPORT_OR_EXPORT_
jint JNICALL JNI_GetCreatedJavaVMs(JavaVM **, jsize, jsize *) {
    PANIC("KiVM does not support JNI_GetCreatedJavaVMs()");
}
