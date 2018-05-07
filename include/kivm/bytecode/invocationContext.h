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

        InstanceKlass *_instanceKlass;

    private:
        static Method* resolveVirtualMethod(oop thisObject, Method *tagMethod);

    private:
        void prepareEnvironment();

        void prepareSynchronized(oop thisObject);

        void finishSynchronized(oop thisObject);

        void invokeNative(bool hasThis, bool resolveTwice);

        void invokeJava(bool hasThis, bool resolveTwice);

    public:
        InvocationContext(JavaThread *thread, Method *method, Stack *stack);

        inline void invoke(bool hasThis, bool resolveTwice) {
            prepareEnvironment();

            if (_method->isNative()) {
                this->invokeNative(hasThis, resolveTwice);

            } else {
                this->invokeJava(hasThis, resolveTwice);
            }
        }
    };
}
