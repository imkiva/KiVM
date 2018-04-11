//
// Created by kiva on 2018/3/27.
//
#pragma once

#include <kivm/oop/instanceKlass.h>
#include <kivm/runtime/thread.h>
#include <kivm/runtime/stack.h>
#include <kivm/runtime/constantPool.h>

namespace kivm {
    class Resolver {
    public:
        static oop resolveJObject(jobject obj);

        static instanceOop tryResolveInstance(jobject obj);

        static typeArrayOop tryResolveTypeArray(jobject obj);

        static objectArrayOop tryResolveObjectArray(jobject object);
    };

    /**
     * Each country has an execution officer,
     * and virtual machines are no exception.
     */
    class Execution {
    public:
        static void putField(RuntimeConstantPool *rt, Stack &stack, int constantIndex);

        static void getField(RuntimeConstantPool *rt, instanceOop receiver, Stack &stack, int constantIndex);

        static void loadIntArrayElement(Stack &stack);

        static void loadFloatArrayElement(Stack &stack);

        static void loadDoubleArrayElement(Stack &stack);

        static void loadLongArrayElement(Stack &stack);

        static void loadObjectArrayElement(Stack &stack);

        static void storeIntArrayElement(Stack &stack);

        static void storeFloatArrayElement(Stack &stack);

        static void storeDoubleArrayElement(Stack &stack);

        static void storeLongArrayElement(Stack &stack);

        static void storeObjectArrayElement(Stack &stack);

        static void loadConstant(RuntimeConstantPool *rt, Stack &stack, int constantIndex);

        static void initializeClass(JavaThread *javaThread, InstanceKlass *klass);

        static void callDefaultConstructor(JavaThread *javaThread, instanceOop oop);

        static void callVoidMethod(JavaThread *javaThread, Method *method, const std::list<oop> &args);

        static bool instanceOf(Klass *ref, Klass *klass);
    };
}
