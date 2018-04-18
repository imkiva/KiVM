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
        Stack &_stack;

        InstanceKlass *_instanceKlass;

    private:
        void prepareEnvironment();

        void prepareSynchronized(oop thisObject);

        void finishSynchronized(oop thisObject);

        void invokeNative(bool hasThis);

        void invokeJava(bool hasThis);

    public:
        InvocationContext(JavaThread *thread, Method *method, Stack &stack);

        void invoke(bool hasThis);
    };
}
