//
// Created by kiva on 2018/4/28.
//

#include <kivm/kivm.h>
#include <kivm/oop/primitiveOop.h>
#include <kivm/oop/arrayOop.h>
#include <kivm/native/classNames.h>
#include <kivm/bytecode/execution.h>
#include <unistd.h>
#include <cstring>
#include <kivm/memory/universe.h>

using namespace kivm;

JAVA_NATIVE void Java_java_io_FileOutputStream_initIDs(JNIEnv *env, jclass java_io_FileOutputStream) {
    D("java/io/FileOutputStream.initIDs()V");
}

JAVA_NATIVE void Java_java_io_FileOutputStream_writeBytes(JNIEnv *env, jobject javaOutputStream,
                                                          jbyteArray b, jint off, jint len, jboolean append) {
    static auto CLASS = (InstanceKlass *) BootstrapClassLoader::get()
        ->loadClass(L"java/io/FileOutputStream");
    static auto FD_CLASS = (InstanceKlass *) BootstrapClassLoader::get()
        ->loadClass(L"java/io/FileDescriptor");
    static auto FD_FIELD = CLASS->getInstanceFieldInfo(CLASS->getName(), L"fd", L"Ljava/io/FileDescriptor;");
    static auto FD_INT_FIELD = FD_CLASS->getInstanceFieldInfo(FD_CLASS->getName(), L"fd", L"I");

    auto byteArray = Resolver::typeArray(b);
    if (byteArray == nullptr) {
        SHOULD_NOT_REACH_HERE();
    }

    if (byteArray->getLength() <= off && byteArray->getLength() < (off + len)) {
        auto thread = Threads::currentThread();
        assert(thread != nullptr);
        // TODO: verbose exception message
        thread->throwException(Global::java_lang_ArrayIndexOutOfBoundsException);
        return;
    }

    auto streamOop = Resolver::instance(javaOutputStream);
    instanceOop fdOop = nullptr;
    if (!streamOop->getFieldValue(FD_FIELD, (oop *) &fdOop)) {
        SHOULD_NOT_REACH_HERE();
    }

    intOop fdInt = nullptr;
    if (!fdOop->getFieldValue(FD_INT_FIELD, (oop *) &fdInt)) {
        SHOULD_NOT_REACH_HERE();
    }

    int fd = fdInt->getValue();
    auto *buf = (char *) Universe::allocCObject(sizeof(char) * len);
    for (int i = off, j = 0; i < off + len; i++, j++) {
        buf[j] = (char) ((intOop) byteArray->getElementAt(i))->getValue();
    }
    if (write(fd, buf, (size_t) len) == -1) {
        auto thread = Threads::currentThread();
        assert(thread != nullptr);
        // TODO: verbose exception message
        thread->throwException(Global::java_io_IOException);
        return;
    }
    Universe::deallocCObject(buf);
}
