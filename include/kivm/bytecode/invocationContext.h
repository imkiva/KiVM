//
// Created by kiva on 2018/4/13.
//
#pragma once

#include <kivm/runtime/thread.h>

namespace kivm {
    class InvocationContext {
    private:
        JavaThread *_thread;
        Method *_method;
        Stack &_stack;

        InstanceKlass *_instanceKlass;
        int _nargs;

    private:
        void prepareInvocation();

    public:
        InvocationContext(JavaThread *thread, Method *method, Stack &stack);

        void invoke(bool hasThis);
    };
}
