//
// Created by kiva on 2018/4/28.
//

#include <kivm/kivm.h>
#include <kivm/native/classNames.h>

using namespace kivm;

JAVA_NATIVE void Java_java_io_FileDescriptor_initIDs(JNIEnv *env, jclass java_io_FileDescriptor) {
    D("java/io/FileDescriptor.initIDs()V");
}

JAVA_NATIVE jlong Java_java_io_FileDescriptor_set(JNIEnv *env, jclass java_io_FileDescriptor, jint fd) {
    // TODO: convert fd to file handle for windows
    return fd;
}
