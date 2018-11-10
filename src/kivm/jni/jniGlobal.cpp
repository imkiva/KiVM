//
// Created by kiva on 2018/4/16.
//
#include <kivm/kivm.h>

_JNI_IMPORT_OR_EXPORT_
jint JNICALL JNI_GetDefaultJavaVMInitArgs(void *args) {
    auto initArgs = (JavaVMInitArgs *) args;
    if (initArgs == nullptr) {
        return JNI_ERR;
    }
    initArgs->version = JNI_VERSION_1_8;
    initArgs->nOptions = 0;
    initArgs->ignoreUnrecognized = JNI_TRUE;
    initArgs->options = nullptr;
    return JNI_OK;
}

_JNI_IMPORT_OR_EXPORT_
jint JNICALL JNI_CreateJavaVM(JavaVM **pvm, void **penv, void *args) {
    auto initArgs = (JavaVMInitArgs *) args;
    return kivm::KiVM::createVirtualMachine(pvm, (JNIEnv **) penv, initArgs);
}

_JNI_IMPORT_OR_EXPORT_
jint JNICALL JNI_GetCreatedJavaVMs(JavaVM **pJavaVM, jsize bufLen, jsize *nVMs) {
    *pJavaVM = kivm::KiVM::getJavaVMQuick();
    *nVMs = 1;
    return JNI_OK;
}
