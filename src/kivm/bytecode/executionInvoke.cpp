//
// Created by kiva on 2018/4/19.
//
#include <kivm/bytecode/execution.h>
#include <kivm/bytecode/invocationContext.h>

namespace kivm {
    void Execution::invokeSpecial(JavaThread *thread, RuntimeConstantPool *rt, Stack &stack, int constantIndex) {
        Method *method = rt->getMethod(constantIndex);
        if (method == nullptr) {
            PANIC("NoSuchMethodError");
        }

        InvocationContext(thread, method, stack).invoke(true);
    }

    void Execution::invokeStatic(JavaThread *thread, RuntimeConstantPool *rt, Stack &stack, int constantIndex) {
        Method *method = rt->getMethod(constantIndex);
        if (method == nullptr) {
            PANIC("NoSuchMethodError");
        }

        if (!method->isStatic() || method->isAbstract()) {
            PANIC("invalid invokeStatic");
        }

        InvocationContext(thread, method, stack).invoke(false);
    }

    void Execution::invokeVirtual(JavaThread *thread, RuntimeConstantPool *rt, Stack &stack, int constantIndex) {
        Method *method = rt->getMethod(constantIndex);
        if (method == nullptr) {
            PANIC("NoSuchMethodError");
        }

        if (method->isStatic()) {
            PANIC("invalid invokeVirtual");
        }

        InvocationContext(thread, method, stack).invoke(true);
    }
}
