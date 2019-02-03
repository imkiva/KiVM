//
// Created by kiva on 2018/4/19.
//
#include <kivm/bytecode/execution.h>
#include <kivm/bytecode/javaCall.h>
#include <kivm/native/classNames.h>
#include <kivm/oop/instanceOop.h>
#include <kivm/oop/mirrorOop.h>
#include <kivm/oop/primitiveOop.h>
#include <kivm/oop/arrayOop.h>
#include <kivm/oop/arrayKlass.h>

namespace kivm {
    static void panicNoSuchMethod(RuntimeConstantPool *rt, int constantIndex) {
        cp_info **pool = rt->getRawPool();
        auto methodRef = requireConstant<CONSTANT_Methodref_info>(pool, constantIndex);
        auto nameAndTypeInfo = requireConstant<CONSTANT_NameAndType_info>(pool, methodRef->name_and_type_index);
        auto classInfo = requireConstant<CONSTANT_Class_info>(pool, methodRef->class_index);

        auto className = requireConstant<CONSTANT_Utf8_info>(pool, classInfo->name_index);
        auto methodName = requireConstant<CONSTANT_Utf8_info>(pool, nameAndTypeInfo->name_index);
        auto methodDesc = requireConstant<CONSTANT_Utf8_info>(pool, nameAndTypeInfo->descriptor_index);

        // TODO: throw an exception
        PANIC("NoSuchMethodError: %s.%s:%s",
            strings::toStdString(className->getConstant()).c_str(),
            strings::toStdString(methodName->getConstant()).c_str(),
            strings::toStdString(methodDesc->getConstant()).c_str());
    }

    static JavaObject("MethodHandle") makeFieldHandle(JavaThread *thread, RuntimeConstantPool *rt,
                                                      instanceOop lookupObject, int kind, int index) {
        if (rt->getRawPool()[index]->tag != CONSTANT_Fieldref) {
            SHOULD_NOT_REACH_HERE();
            return nullptr;
        }

        pools::FieldPoolEntry field = nullptr;
        Method *method = nullptr;

        if (kind == REF_getField || kind == REF_putField) {
            field = rt->getInstanceField(index);
            method = lookupObject->getInstanceClass()->getThisClassMethod(L"findGetter",
                L"(Ljava/lang/Class;Ljava/lang/String;Ljava/lang/Class;)Ljava/lang/invoke/MethodHandle;");
        } else {
            field = rt->getStaticField(index);
            method = lookupObject->getInstanceClass()->getThisClassMethod(L"findStaticGetter",
                L"(Ljava/lang/Class;Ljava/lang/String;Ljava/lang/Class;)Ljava/lang/invoke/MethodHandle;");

        }

        auto f = field->_field;
        auto mh = JavaCall::withArgs(thread, method,
            {lookupObject,
             f->getClass()->getJavaMirror(),
             java::lang::String::from(f->getName()),
             f->getValueTypeClass()->getJavaMirror()});
        return Resolver::instance(mh);
    }

    static JavaObject("MethodHandles.Lookup") makeMethodHandlesLookup(JavaThread *thread) {
        static String lookupMethodName = L"lookup";
        static String lookupMethodDesc = L"()Ljava/lang/invoke/MethodHandles$Lookup;";

        // TODO
        auto klass = (InstanceKlass *) BootstrapClassLoader::get()
            ->loadClass(L"java/lang/invoke/MethodHandles");
        auto lookupMethod = klass->getThisClassMethod(lookupMethodName, lookupMethodDesc);
        auto lookupObject = JavaCall::withArgs(thread, lookupMethod, {});
        if (thread->isExceptionOccurred()) {
            KiVM::printStackTrace(thread);
            SHOULD_NOT_REACH_HERE_M("wtf");
        }
        return Resolver::instance(lookupObject);
    }

    static JavaObject("MethodType") makeMethodType(JavaThread *thread, const std::vector<mirrorOop> &args,
                                                   mirrorOop retType) {
        auto classArrayClass = (ObjectArrayKlass *) BootstrapClassLoader::get()
            ->loadClass(L"[Ljava/lang/Class;");
        auto array = classArrayClass->newInstance(int(args.size()));
        for (int i = 0; i < args.size(); ++i) {
            array->setElementAt(i, args[i]);
        }

        auto methodTypeClass = (InstanceKlass *) BootstrapClassLoader::get()
            ->loadClass(L"java/lang/invoke/MethodType");
        if (methodTypeClass == nullptr) {
            SHOULD_NOT_REACH_HERE();
        }

        auto ctor = methodTypeClass->getThisClassMethod(L"methodType",
            L"(Ljava/lang/Class;[Ljava/lang/Class;)Ljava/lang/invoke/MethodType;");
        if (ctor == nullptr) {
            SHOULD_NOT_REACH_HERE();
        }

        auto mt = JavaCall::withArgs(thread, ctor, {retType, array});
        return Resolver::instance(mt);
    }

    static JavaObject("MethodType") makeMethodType(JavaThread *thread, const String &descriptor) {
        auto args = parseArguments(descriptor);
        auto retType = parseReturnType(descriptor);
        return makeMethodType(thread, args, retType);
    }

    static JavaObject("MethodType") makeMethodType(JavaThread *thread, Method *method) {
        auto &args = method->getArgumentClassTypes();
        auto retType = method->getReturnClassType();
        return makeMethodType(thread, args, retType);
    }

    static JavaObject("MethodHandle") makeInvokeHandle(JavaThread *thread, RuntimeConstantPool *rt,
                                                       instanceOop lookupObject, int kind, int index) {
        if (rt->getRawPool()[index]->tag != CONSTANT_Methodref
            && rt->getRawPool()[index]->tag != CONSTANT_InterfaceMethodref) {
            SHOULD_NOT_REACH_HERE();
            return nullptr;
        }

        if (kind == REF_invokeSpecial || kind == REF_newInvokeSpecial) {
            PANIC("not supported");
        }

        auto targetMethod = rt->getMethod(index);
        Method *findMethod = nullptr;
        switch (kind) {
            case REF_invokeVirtual:
                findMethod = lookupObject->getInstanceClass()->getVirtualMethod(L"findVirtual",
                    L"(Ljava/lang/Class;Ljava/lang/String;Ljava/lang/invoke/MethodType;)Ljava/lang/invoke/MethodHandle;");
                break;
            case REF_invokeInterface:
                findMethod = lookupObject->getInstanceClass()->getThisClassMethod(L"findVirtual",
                    L"(Ljava/lang/Class;Ljava/lang/String;Ljava/lang/invoke/MethodType;)Ljava/lang/invoke/MethodHandle;");
                break;
            case REF_invokeStatic:
                if (targetMethod->getName() == L"<init>" || targetMethod->getName() == L"<clinit>") {
                    SHOULD_NOT_REACH_HERE();
                    return nullptr;
                }

                findMethod = lookupObject->getInstanceClass()->getVirtualMethod(L"findStatic",
                    L"(Ljava/lang/Class;Ljava/lang/String;Ljava/lang/invoke/MethodType;)Ljava/lang/invoke/MethodHandle;");
                break;
            default:
                SHOULD_NOT_REACH_HERE();
                return nullptr;
        }

        auto mh = JavaCall::withArgs(thread, findMethod,
            {lookupObject,
             targetMethod->getClass()->getJavaMirror(),
             java::lang::String::from(targetMethod->getName()),
             makeMethodType(thread, targetMethod)});
        return Resolver::instance(mh);
    }

    static JavaObject("MethodHandle")
    makeMethodHandle(JavaThread *thread, RuntimeConstantPool *rt, int index, bool isBootstrap) {
        auto lookupObject = makeMethodHandlesLookup(thread);
        if (lookupObject == nullptr) {
            SHOULD_NOT_REACH_HERE();
            return nullptr;
        }

        if (rt->getRawPool()[index]->tag != CONSTANT_MethodHandle) {
            SHOULD_NOT_REACH_HERE();
            return nullptr;
        }

        auto methodHandle = (CONSTANT_MethodHandle_info *) rt->getRawPool()[index];
        auto refIndex = methodHandle->reference_index;
        auto refKind = methodHandle->reference_kind;

        if (isBootstrap) {
            switch (refKind) {
                case REF_invokeStatic:
                case REF_newInvokeSpecial:
                    break;
                default:
                    SHOULD_NOT_REACH_HERE();
                    return nullptr;
            }
        }

        switch (refKind) {
            case REF_getField:
            case REF_getStatic:
            case REF_putField:
            case REF_putStatic:
                return makeFieldHandle(thread, rt, lookupObject, refKind, refIndex);

            case REF_invokeVirtual:
            case REF_invokeStatic:
            case REF_invokeSpecial:
            case REF_newInvokeSpecial:
            case REF_invokeInterface:
                return makeInvokeHandle(thread, rt, lookupObject, refKind, refIndex);

            default:
                SHOULD_NOT_REACH_HERE();
        }

        return nullptr;
    }

    oop Execution::invokeSpecial(JavaThread *thread, RuntimeConstantPool *rt, Stack &stack, int constantIndex) {
        Method *method = rt->getMethod(constantIndex);
        if (method == nullptr) {
            panicNoSuchMethod(rt, constantIndex);
            return nullptr;
        }

        return JavaCall::withStack(thread, method, &stack, true);
    }

    oop Execution::invokeStatic(JavaThread *thread, RuntimeConstantPool *rt, Stack &stack, int constantIndex) {
        Method *method = rt->getMethod(constantIndex);
        if (method == nullptr) {
            panicNoSuchMethod(rt, constantIndex);
            return nullptr;
        }

        if (!method->isStatic() || method->isAbstract()) {
            PANIC("invalid invokeStatic");
        }

        return JavaCall::withStack(thread, method, &stack, true);
    }

    oop Execution::invokeVirtual(JavaThread *thread, RuntimeConstantPool *rt, Stack &stack, int constantIndex) {
        Method *method = rt->getMethod(constantIndex);
        if (method == nullptr) {
            panicNoSuchMethod(rt, constantIndex);
            return nullptr;
        }

        if (method->isStatic()) {
            PANIC("invalid invokeVirtual");
        }

        // abstract methods need to be resolve by name
        // but currently we cannot get exact method
        // until we got `this` object
        return JavaCall::withStack(thread, method, &stack);
    }

    oop Execution::invokeInterface(JavaThread *thread, RuntimeConstantPool *rt, Stack &stack,
                                   int constantIndex, int count) {
        // Do not use invokeVirtual()
        // we need rt->getMethod()
        Method *method = rt->getMethod(constantIndex);
        if (method == nullptr) {
            panicNoSuchMethod(rt, constantIndex);
            return nullptr;
        }

        if (method->isStatic()) {
            PANIC("invalid invokeInterface");
        }

        // interface methods need to be resolve by name
        // but currently we cannot get exact method
        // until we got `this` object
        return JavaCall::withStack(thread, method, &stack);
    }

    oop Execution::invokeDynamic(JavaThread *thread, InstanceKlass *klass,
                                 Stack &stack, int constantIndex) {
        // TODO: invoke dynamic
        auto invoke = klass->getRuntimeConstantPool()->getInvokeDynamic(constantIndex);
        if (invoke == nullptr) {
            SHOULD_NOT_REACH_HERE_M("Illegal invokedynamic, please check your Java compiler");
        }

        auto bmTable = klass->getBootstrapMethodTable();
        if (bmTable == nullptr) {
            SHOULD_NOT_REACH_HERE();
        }

        auto methodIndex = invoke->methodIndex;
        if (methodIndex >= bmTable->num_bootstrap_methods) {
            SHOULD_NOT_REACH_HERE_M("Illegal invokedynamic, please check your Java compiler");
        }

        auto bootstrap = bmTable->bootstrap_methods[methodIndex];
        auto rt = klass->getRuntimeConstantPool();
        const String &descriptor = *invoke->methodNameAndType->second;

        auto methodHandleIndex = bootstrap.bootstrap_method_ref;
        auto methodHandle = makeMethodHandle(thread, rt, methodHandleIndex, true);

        JavaObject("Lookup") lookupObject = makeMethodHandlesLookup(thread);
        std::list<oop> callSiteArgs;
        callSiteArgs.push_back(lookupObject);
        callSiteArgs.push_back(java::lang::String::from(*invoke->methodNameAndType->first));
        callSiteArgs.push_back(makeMethodType(thread, descriptor));

        for (int i = 0; i < bootstrap.num_bootstrap_arguments; ++i) {
            int argIndex = bootstrap.bootstrap_arguments[i];
            switch (rt->getRawPool()[argIndex]->tag) {
                case CONSTANT_String:
                    callSiteArgs.push_back(rt->getString(argIndex));
                    break;
                case CONSTANT_Class:
                    callSiteArgs.push_back(rt->getClass(argIndex)->getJavaMirror());
                    break;
                case CONSTANT_Integer:
                    callSiteArgs.push_back(new intOopDesc(rt->getInt(argIndex)));
                    break;
                case CONSTANT_Float:
                    callSiteArgs.push_back(new floatOopDesc(rt->getFloat(argIndex)));
                    break;
                case CONSTANT_Long:
                    callSiteArgs.push_back(new longOopDesc(rt->getLong(argIndex)));
                    break;
                case CONSTANT_Double:
                    callSiteArgs.push_back(new doubleOopDesc(rt->getDouble(argIndex)));
                    break;
                case CONSTANT_MethodHandle:
                    callSiteArgs.push_back(makeMethodHandle(thread, rt, argIndex, false));
                    break;
                case CONSTANT_MethodType: {
                    auto methodType = (CONSTANT_MethodType_info *) rt->getRawPool()[argIndex];
                    int descIndex = methodType->descriptor_index;
                    auto desc = rt->getUtf8(descIndex);
                    callSiteArgs.push_back(makeMethodType(thread, *desc));
                    break;
                }
                default:
                    SHOULD_NOT_REACH_HERE();
            }
        }

        auto invokeWithArgsMethod = methodHandle->getInstanceClass()
            ->getVirtualMethod(L"invokeWithArguments", L"([Ljava/lang/Object;)Ljava/lang/Object;");
        if (invokeWithArgsMethod == nullptr) {
            SHOULD_NOT_REACH_HERE();
        }

        auto arrayClass = (ObjectArrayKlass *) BootstrapClassLoader::get()
            ->loadClass(L"[Ljava/lang/Object;");
        if (arrayClass == nullptr) {
            SHOULD_NOT_REACH_HERE();
        }

        auto callSiteArgArray = arrayClass->newInstance(int(callSiteArgs.size()));
        int i = 0;
        for (auto a : callSiteArgs) {
            callSiteArgArray->setElementAt(i++, a);
        }

        // Finally we got the java.lang.invoke.CallSite
        JavaObject("CallSite") callSite = (instanceOop) JavaCall::withArgs(thread, invokeWithArgsMethod,
            {methodHandle, callSiteArgArray});

        auto dynamicInvokerMethod = callSite->getInstanceClass()
            ->getVirtualMethod(L"dynamicInvoker", L"()Ljava/lang/invoke/MethodHandle;");
        if (dynamicInvokerMethod == nullptr) {
            SHOULD_NOT_REACH_HERE();
        }

        JavaObject("MethodHandle") MH = (instanceOop) JavaCall::withArgs(thread, dynamicInvokerMethod,
            {callSite});
        if (MH == nullptr) {
            SHOULD_NOT_REACH_HERE();
        }

        auto invokeExactMethod = MH->getInstanceClass()
            ->getVirtualMethod(L"invokeExact", L"([Ljava/lang/Object;)Ljava/lang/Object;");
        if (invokeExactMethod == nullptr) {
            SHOULD_NOT_REACH_HERE();
        }

        return JavaCall::withMethodHandle(thread, invokeExactMethod, &stack, MH, descriptor);
    }
}
