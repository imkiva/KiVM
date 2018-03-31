//
// Created by kiva on 2018/3/31.
//
#pragma once

#include <kivm/classfile/constantPool.h>
#include <kivm/oop/oopfwd.h>
#include <kivm/classLoader.h>
#include <unordered_map>

namespace kivm {
    class InstanceKlass;

    class StringTable {
    private:
        // hash -> string
        std::unordered_map<int, instanceOop> _pool;

    public:
        static StringTable* getGlobal();

        instanceOop findOrNew(const kivm::String &string);
    };

    class ClassTable {};

    class MethodTable {};

    class FieldTable {};

    class RuntimeConstantPool {
    private:
        cp_info **_constant_pool;
        ClassLoader *_class_loader;

        StringTable _string_pool;
        MethodTable _method_pool;
        FieldTable _field_pool;
        ClassTable _class_pool;

    public:
        explicit RuntimeConstantPool(InstanceKlass *instanceKlass, cp_info **pool);
    };
}
