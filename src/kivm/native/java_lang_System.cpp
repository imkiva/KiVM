//
// Created by kiva on 2018/4/21.
//

#include <compileTimeConfig.h>
#include <kivm/kivm.h>
#include <kivm/native/classNames.h>
#include <kivm/bytecode/execution.h>
#include <kivm/oop/arrayKlass.h>
#include <kivm/oop/arrayOop.h>
#include <kivm/bytecode/invocationContext.h>

using namespace kivm;

JAVA_NATIVE jobject
Java_java_lang_System_initProperties(JNIEnv *env, jclass java_lang_System, jobject propertiesObject) {
    static std::unordered_map<String, String> PROPS{
        {L"java.vm.specification.name",  L"Java Virtual Machine Specification"},
        {L"java.vm.specification.version",  L"1.8"},
        {L"java.vm.version",  L"0.1.0"},
        {L"java.vm.name",  L"Kiva's Java VM"},
        {L"java.vm.info",  L"interpreted mode, sharing"},
        {L"java.specification.version",  L"1.8"},
        {L"java.version",  L"1.8.0-debug"},
        {L"java.runtime.name",  L"Java(TM) SE Runtime Environment"},
        {L"java.runtime.version",  L"1.8.0"},
        {L"gopherProxySet",  L"false"},
        {L"java.vm.vendor",  L"imKiva"},
        {L"java.vendor.url",  L"https://github.com/imkiva"},
        {L"path.separator",  Global::PATH_SEPARATOR},
        {L"file.encoding.pkg",  L"sun.io"},
        {L"user.country",  L"CN"},
        {L"user.language",  L"zh"},
        {L"sun.java.launcher",  L"KIVM_LAUNCHER"},
        {L"sun.os.patch.level",  L"unknown"},
        {L"os.arch",  KIVM_ARCH_NAME},
        {L"sun.arch.data.model",  L"64"},
        {L"line.separator",  L"\n"},
        {L"file.separator",  Global::PATH_DELIMITER},
        {L"sun.jnu.encoding",  L"UTF-8"},
        {L"file.encoding",  L"UTF-8"},
        {L"java.specification.name",  L"Java Platform API Specification"},
        {L"java.class.version",  L"52.0"},
        {L"sun.management.compiler",  L"nop"},
        {L"sun.io.unicode.encoding",  L"UnicodeBig"},
        {L"java.home",  L"."},
        {L"java.class.path",  L"."},
    };

    auto propOop = Resolver::instance(propertiesObject);
    auto put = ((InstanceKlass *) propOop->getInstanceClass())->getVirtualMethod(
        L"put", L"(Ljava/lang/Object;Ljava/lang/Object;)Ljava/lang/Object;");

    auto thread = (JavaThread *) Threads::currentThread();
    InvocationContext::invokeWithArgs(thread, put, {});
    return propertiesObject;
}

JAVA_NATIVE void Java_java_lang_System_arraycopy(JNIEnv *env, jclass java_lang_System,
                                                 jobject javaSrc, jint srcPos,
                                                 jobject javaDest, jint destPos,
                                                 jint length) {
    auto srcOop = Resolver::array(javaSrc);
    auto destOop = Resolver::array(javaDest);

    if (srcOop == nullptr || destOop == nullptr) {
        PANIC("java.lang.NullPointerException");
    }

    auto arrayClass = (ArrayKlass *) srcOop->getClass();
    arrayClass->copyArrayTo(srcOop, destOop, srcPos, destPos, length);
}

JAVA_NATIVE void Java_java_lang_System_setIn0(JNIEnv *env, jclass java_lang_System, jobject javaInputStream) {
    auto inputStreamOop = Resolver::instance(javaInputStream);
    auto system = Resolver::instanceClass(java_lang_System);
    system->setStaticFieldValue(J_SYSTEM, L"in", L"Ljava/io/InputStream;", inputStreamOop);
}

JAVA_NATIVE void Java_java_lang_System_setOut0(JNIEnv *env, jclass java_lang_System, jobject javaPrintStream) {
    auto printStreamOop = Resolver::instance(javaPrintStream);
    auto system = Resolver::instanceClass(java_lang_System);
    system->setStaticFieldValue(J_SYSTEM, L"out", L"Ljava/io/PrintStream;", printStreamOop);
}

JAVA_NATIVE void Java_java_lang_System_setErr0(JNIEnv *env, jclass java_lang_System, jobject javaPrintStream) {
    auto printStreamOop = Resolver::instance(javaPrintStream);
    auto system = Resolver::instanceClass(java_lang_System);
    system->setStaticFieldValue(J_SYSTEM, L"err", L"Ljava/io/PrintStream;", printStreamOop);
}
