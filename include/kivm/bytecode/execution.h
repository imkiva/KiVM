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
        static void initializeClass(JavaThread *javaThread, InstanceKlass *klass);

        static void callDefaultConstructor(JavaThread *javaThread, instanceOop oop);

        static void callVoidMethod(JavaThread *javaThread, Method *method, const std::list<oop> &args);

        static bool instanceOf(Klass *ref, Klass *klass);
    };
}
