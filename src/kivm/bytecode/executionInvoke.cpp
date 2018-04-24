//
// Created by kiva on 2018/4/19.
//
#include <kivm/bytecode/execution.h>
#include <kivm/bytecode/invocationContext.h>

namespace kivm {
    static void panicNoSuchMethod(RuntimeConstantPool *rt, int constantIndex) {
        cp_info **pool = rt->getRawPool();
        auto methodRef = requireConstant<CONSTANT_Methodref_info>(pool, constantIndex);
        auto nameAndTypeInfo = requireConstant<CONSTANT_NameAndType_info>(pool, methodRef->name_and_type_index);
        auto classInfo = requireConstant<CONSTANT_Class_info>(pool, methodRef->class_index);

        auto className = requireConstant<CONSTANT_Utf8_info>(pool, classInfo->name_index);
        auto methodName = requireConstant<CONSTANT_Utf8_info>(pool, nameAndTypeInfo->name_index);
        auto methodDesc = requireConstant<CONSTANT_Utf8_info>(pool, nameAndTypeInfo->descriptor_index);

        PANIC("NoSuchMethodError: %s.%s:%s",
              strings::toStdString(className->getConstant()).c_str(),
              strings::toStdString(methodName->getConstant()).c_str(),
              strings::toStdString(methodDesc->getConstant()).c_str());
    }

    void Execution::invokeSpecial(JavaThread *thread, RuntimeConstantPool *rt, Stack &stack, int constantIndex) {
        Method *method = rt->getMethod(constantIndex);
        if (method == nullptr) {
            panicNoSuchMethod(rt, constantIndex);
        }

        InvocationContext(thread, method, stack).invoke(true);
    }

    void Execution::invokeStatic(JavaThread *thread, RuntimeConstantPool *rt, Stack &stack, int constantIndex) {
        Method *method = rt->getMethod(constantIndex);
        if (method == nullptr) {
            panicNoSuchMethod(rt, constantIndex);
            return;
        }

        if (!method->isStatic() || method->isAbstract()) {
            PANIC("invalid invokeStatic");
        }

        InvocationContext(thread, method, stack).invoke(false);
    }

    void Execution::invokeVirtual(JavaThread *thread, RuntimeConstantPool *rt, Stack &stack, int constantIndex) {
        Method *method = rt->getMethod(constantIndex);
        if (method == nullptr) {
            panicNoSuchMethod(rt, constantIndex);
            return;
        }

        if (method->isStatic()) {
            PANIC("invalid invokeVirtual");
        }

        InvocationContext(thread, method, stack).invoke(true);
    }

    void Execution::invokeInterface(JavaThread *thread, RuntimeConstantPool *rt, Stack &stack, int constantIndex) {
        Execution::invokeVirtual(thread, rt, stack, constantIndex);
    }
}
