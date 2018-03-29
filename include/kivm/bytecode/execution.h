//
// Created by kiva on 2018/3/27.
//
#pragma once

#include <kivm/oop/instanceKlass.h>
#include <kivm/runtime/thread.h>

namespace kivm {
    /**
     * Each country has an execution officer,
     * and virtual machines are no exception.
     */
    class Execution {
    public:
        static void initialize_class(JavaThread *javaThread, InstanceKlass *klass);

        static void call_default_constructor(JavaThread *javaThread, instanceOop oop);

        static void call_void_method(JavaThread *javaThread, Method *method, const std::list<oop> &args);

        static bool instanceOf(Klass *ref, Klass *klass);
    };
}
