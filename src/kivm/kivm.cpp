//
// Created by kiva on 2018/4/16.
//
#include <kivm/kivm.h>
#include <kivm/jni/jniJavaVM.h>

namespace kivm {
    int KiVM::createVirtualMachine(JavaVM **pJavaVM) {
        // TODO: create VM
        auto invokeInterface = new JNIInvokeInterface_;
        invokeInterface->AttachCurrentThread = JVM_ENTRY_NAME(AttachCurrentThread);
        invokeInterface->AttachCurrentThreadAsDaemon = JVM_ENTRY_NAME(AttachCurrentThreadAsDaemon);
        invokeInterface->DestroyJavaVM = JVM_ENTRY_NAME(DestroyJavaVM);
        invokeInterface->DetachCurrentThread = JVM_ENTRY_NAME(DetachCurrentThread);
        invokeInterface->GetEnv = JVM_ENTRY_NAME(GetEnv);

        auto javaVM = new JavaVM;
        javaVM->functions = invokeInterface;
        *pJavaVM = javaVM;
        return JNI_OK;
    }
}
