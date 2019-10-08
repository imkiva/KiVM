//
// Created by kiva on 2019/9/17.
//

#include <kivm/kivm.h>
#include <kivm/oop/arrayOop.h>
#include <kivm/oop/primitiveOop.h>
#include <kivm/oop/mirrorOop.h>
#include <kivm/bytecode/javaCall.h>
#include <kivm/bytecode/execution.h>
#include <kivm/native/java_lang_reflect_Method.h>

#include <list>

using namespace kivm;

JAVA_NATIVE jobject Java_sun_reflect_NativeMethodAccessorImpl_invoke0(JNIEnv *env, jclass cls,
                                                                      jobject javaMethod,
                                                                      jobject javaThis,
                                                                      jobjectArray javaArguments) {
    auto thread = Threads::currentThread();

    auto methodOop = Resolver::instance(javaMethod);
    auto thisOop = Resolver::javaOop(javaThis);

    mirrorOop targetClass;
    if (!methodOop->getFieldValue(java::lang::reflect::Method::FIELD_CLAZZ, (oop *) &targetClass)) {
        // TODO: throw InternalException
        SHOULD_NOT_REACH_HERE();
    }

    intOop targetSlot;
    if (!methodOop->getFieldValue(java::lang::reflect::Method::FIELD_SLOT, (oop *) &targetSlot)) {
        // TODO: throw InternalException
        SHOULD_NOT_REACH_HERE();
    }

    if (targetClass->getTarget() != nullptr
        && targetClass->getTarget()->getClassType() == ClassType::INSTANCE_CLASS) {
        // Reflection invocation on InstanceKlass
        auto *klass = (InstanceKlass *) targetClass->getTarget();
        int slot = targetSlot->getValue();

        Method *method = klass->getDeclaredMethodByOffset(slot);

        auto argsArray = Resolver::objectArray(javaArguments);
        if (argsArray->getLength() != method->getArgumentValueTypes().size()) {
            auto exClass = (InstanceKlass *) BootstrapClassLoader::get()
                ->loadClass(L"java/lang/IllegalArgumentException");
            thread->throwException(exClass, L"wrong number of arguments", false);
            return nullptr;
        }

        std::list<oop> args;

        // push this if method is not static
        if (thisOop != nullptr && !method->isStatic()) {
            args.push_back(thisOop);
        }

        for (int i = 0; i < argsArray->getLength(); ++i) {
            args.push_back(argsArray->getElementAt(i));
        }

        return JavaCall::withArgs(thread, method, args);
    }

    // TODO: support invocation on other class types
    SHOULD_NOT_REACH_HERE_M("TODO: support invocation on other class types");
    return nullptr;
}
