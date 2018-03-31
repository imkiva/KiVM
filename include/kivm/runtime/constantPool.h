//
// Created by kiva on 2018/3/31.
//
#pragma once

#include <kivm/classfile/constantPool.h>
#include <kivm/oop/oopfwd.h>
#include <kivm/classLoader.h>
#include <unordered_map>

namespace kivm {
    class Klass;

    class InstanceKlass;

    class RuntimeConstantPool;

    template<typename T, typename Creator>
    class ConstantTable {
        friend class RuntimeConstantPool;

    private:
        cp_info **_raw_pool = nullptr;

        // constant-pool-index -> constant
        std::unordered_map<int, T> _pool;

    public:
        virtual T findOrNew(int index) = 0;
    };

    class RuntimeConstantPool {
    private:
        ClassLoader *_class_loader;

    public:
        explicit RuntimeConstantPool(InstanceKlass *instanceKlass);

        void attachConstantPool(cp_info **pool) {
        }
    };
}
