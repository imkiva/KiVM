//
// Created by kiva on 2018/5/5.
//

#include <kivm/kivm.h>
#include <kivm/bytecode/execution.h>
#include <kivm/native/classNames.h>
#include <kivm/runtime/frameWalker.h>
#include <kivm/oop/arrayKlass.h>
#include <kivm/oop/arrayOop.h>
#include <kivm/bytecode/invocationContext.h>
#include <kivm/oop/primitiveOop.h>

using namespace kivm;

JAVA_NATIVE jobject Java_java_lang_Throwable_fillInStackTrace(JNIEnv *env, jobject javaThrowable, jint depth) {
    static auto ARRAY_CLASS = (ObjectArrayKlass *) BootstrapClassLoader::get()
        ->loadClass(L"[Ljava/lang/StackTraceElement;");
    static auto ELEMENT_CLASS = (InstanceKlass *) BootstrapClassLoader::get()
        ->loadClass(L"java/lang/StackTraceElement");
    static auto ELEMENT_CTOR = ELEMENT_CLASS->getThisClassMethod(L"<init>",
        L"(Ljava/lang/String;Ljava/lang/String;Ljava/lang/String;I)V");

    auto throwableOop = Resolver::instance(javaThrowable);
    auto thread = Threads::currentThread();
    assert(thread != nullptr);

    FrameWalker walker(thread);
    auto stackTraceArray = ARRAY_CLASS->newInstance(walker.getSize());

    int position = 0;
    for (auto iter = walker.begin(); *iter; ++iter, ++position) {
        auto method = iter->getMethod();
        auto pc = iter->getReturnPc();

        // native method
        int lineNumber = -2;
        if (!iter->isNativeFrame()) {
            lineNumber = method->getLineNumber(pc);
        }

        auto element = ELEMENT_CLASS->newInstance();
        InvocationContext::invokeWithArgs(thread, ELEMENT_CTOR, {
            element,
            java::lang::String::from(strings::replaceAll(method->getClass()->getName(),
                Global::SLASH, Global::DOT)),
            java::lang::String::from(method->getName()),
            java::lang::String::from(method->getClass()->getSourceFile()),
            new intOopDesc(lineNumber)
        });
        stackTraceArray->setElementAt(position, element);
    }

    throwableOop->setFieldValue(L"java/lang/Throwable",
        L"stackTrace", L"[Ljava/lang/StackTraceElement;",
        nullptr);
    throwableOop->setFieldValue(L"java/lang/Throwable",
        L"backtrace", L"Ljava/lang/Object;",
        stackTraceArray);
    return throwableOop;
}

JAVA_NATIVE jint Java_java_lang_Throwable_getStackTraceDepth(JNIEnv *env, jobject javaThrowable) {
    auto throwableOop = Resolver::instance(javaThrowable);
    arrayOop stackTraceArray = nullptr;
    if (throwableOop->getFieldValue(L"java/lang/Throwable",
        L"backtrace", L"Ljava/lang/Object;",
        (oop *) &stackTraceArray)) {
        return stackTraceArray->getLength();
    }
    return 0;
}

JAVA_NATIVE jobject Java_java_lang_Throwable_getStackTraceElement(JNIEnv *env, jobject javaThrowable, jint index) {
    auto throwableOop = Resolver::instance(javaThrowable);
    arrayOop stackTraceArray = nullptr;
    if (throwableOop->getFieldValue(L"java/lang/Throwable",
        L"backtrace", L"Ljava/lang/Object;",
        (oop *) &stackTraceArray)) {
        if (index >= 0 && index < stackTraceArray->getLength()) {
            return stackTraceArray->getElementAt(index);
        }
    }
    return nullptr;
}
