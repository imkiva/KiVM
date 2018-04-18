//
// Created by kiva on 2018/4/19.
//
#include <kivm/bytecode/execution.h>
#include <kivm/bytecode/invocationContext.h>
#include <kivm/oop/instanceOop.h>
#include <kivm/oop/primitiveOop.h>
#include <kivm/oop/arrayOop.h>
#include <kivm/method.h>

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
}
