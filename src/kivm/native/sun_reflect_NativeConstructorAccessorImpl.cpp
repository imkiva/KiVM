//
// Created by kiva on 2018/5/24.
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

JAVA_NATIVE jobject Java_sun_reflect_NativeConstructorAccessorImpl_newInstance0(JNIEnv *env, jclass cls,
                                                                                jobject javaConstructor,
                                                                                jobjectArray javaArguments) {
    auto thread = Threads::currentThread();

    auto ctorOop = Resolver::instance(javaConstructor);
    auto targetClass = getClassFromConstructor(ctorOop);
    auto slot = getSlotFromConstructor(ctorOop);
    auto mirrorTarget = (InstanceKlass *) targetClass->getMirrorTarget();

    auto instance = mirrorTarget->newInstance();
    auto ctorMethod = mirrorTarget->getDeclaredMethodByOffset(slot);

    if (ctorMethod->getName() != L"<init>") {
        SHOULD_NOT_REACH_HERE();
    }

    std::list<oop> callingArgs;
    callingArgs.push_back(instance);

    if (javaArguments != nullptr) {
        auto arguments = Resolver::objectArray(javaArguments);
        for (int i = 0; i < arguments->getLength(); ++i) {
            callingArgs.push_back(arguments->getElementAt(i));
        }
    }
    InvocationContext::invokeWithArgs(thread, ctorMethod, callingArgs, true);
    return instance;
}
