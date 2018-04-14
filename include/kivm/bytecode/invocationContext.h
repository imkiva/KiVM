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

    public:
        InvocationContext(JavaThread *thread, Method *method, Stack &stack);

        void invoke(bool hasThis);

        void invokeNative(bool hasThis);
    };
}
