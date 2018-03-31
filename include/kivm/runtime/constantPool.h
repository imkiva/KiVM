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
    public:
        static instanceOop findOrNew(const kivm::String &string);
    };

    class RuntimeConstantPool {
    private:
        cp_info **_constant_pool;
        ClassLoader *_class_loader;

    public:
        explicit RuntimeConstantPool(InstanceKlass *instanceKlass, cp_info **pool);
    };
}
