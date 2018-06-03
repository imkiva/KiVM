//
// Created by kiva on 2018/5/26.
//

#include <kivm/kivm.h>
#include <kivm/runtime/abstractThread.h>
#include <kivm/native/classNames.h>
#include <kivm/bytecode/execution.h>
#include <kivm/oop/mirrorOop.h>
#include <kivm/oop/instanceKlass.h>
#include <kivm/oop/arrayOop.h>
#include <kivm/bytecode/invocationContext.h>

using namespace kivm;

JAVA_NATIVE jobject Java_sun_nio_cs_StreamEncoder_forOutputStreamWriter(JNIEnv *env, jclass sun_nio_cs_StreamEncoder,
                                                                        jobject javaOutputStream,
                                                                        jobject javaObject,
                                                                        jstring javaCharsetName) {
    auto thread = (JavaThread *) Threads::currentThread();

    auto encoder = (InstanceKlass *) BootstrapClassLoader::get()
        ->loadClass(L"sun/nio/cs/StreamEncoder");
    auto method = encoder->getThisClassMethod(L"<init>",
        L"(Ljava/io/OutputStream;Ljava/lang/Object;Ljava/nio/charset/Charset;)V");
    auto encoderOop = encoder->newInstance();
    InvocationContext::invokeWithArgs(thread, method,
        {
            encoderOop,
            Resolver::instance(javaOutputStream),
            Resolver::instance(javaObject),
            Global::DEFAULT_UTF8_CHARSET
        }, true);
    return encoderOop;
}
