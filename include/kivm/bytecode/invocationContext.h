//
// Created by kiva on 2018/4/13.
//
#pragma once

#include <kivm/runtime/thread.h>
#include <list>

namespace kivm {
    class InvocationContext {
    private:
        JavaThread *_thread;
        Method *_method;
        Stack *_stack;
        std::list<oop> _args;
        bool _obtainArgsFromStack;

        InstanceKlass *_instanceKlass;

    private:
        static Method *resolveVirtualMethod(oop thisObject, Method *tagMethod);

    private:
        void prepareEnvironment();

        void prepareSynchronized(oop thisObject);

        void finishSynchronized(oop thisObject);

        oop invokeNative(bool hasThis, bool resolveTwice);

        oop invokeJava(bool hasThis, bool resolveTwice);

        inline oop invoke(bool hasThis, bool resolveTwice) {
            prepareEnvironment();

            if (_method->isNative()) {
                return this->invokeNative(hasThis, resolveTwice);

            } else {
                return this->invokeJava(hasThis, resolveTwice);
            }
        }

        InvocationContext(JavaThread *thread, Method *method, Stack *stack);

        InvocationContext(JavaThread *thread, Method *method, const std::list<oop> &args);

    public:
        static inline oop invokeWithArgs(JavaThread *thread, Method *method, const std::list<oop> &args) {
            return InvocationContext(thread, method, args).invoke(false, false);
        }

        static inline oop invokeWithStack(JavaThread *thread, Method *method, Stack *stack,
                                           bool hasThis, bool resolveTwice) {
            return InvocationContext(thread, method, stack).invoke(hasThis, resolveTwice);
        }
    };
}
