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

        oop callInterpreter();

        inline oop invoke() {
            prepareEnvironment();
            bool hasThis = !_method->isStatic();
            bool resolveTwice = _method->isAbstract()
                                || (_method->isPublic() && !_method->isFinal());

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
            if (method->isNative()) {
                PANIC("InvocationContext: directly invoke native methods with args is not supported yet");
            }
            return InvocationContext(thread, method, args).invoke();
        }

        static inline oop invokeWithStack(JavaThread *thread, Method *method, Stack *stack) {
            return InvocationContext(thread, method, stack).invoke();
        }
    };
}
