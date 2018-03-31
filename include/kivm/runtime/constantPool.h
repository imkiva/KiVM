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

    namespace constant_table {
        template<typename T, typename EntryType, typename Creator, int CONSTANT_TAG>
        class ConstantTable {
            friend class RuntimeConstantPool;

        private:
            cp_info **_raw_pool = nullptr;

            // constant-pool-index -> constant
            std::unordered_map<int, T> _pool;

        public:
            T findOrNew(int index) {
                if (_raw_pool[index]->tag != CONSTANT_TAG) {
                    PANIC("Accessing an incompatible constant entry");
                }
                const auto &iter = _pool.find(index);
                if (iter != _pool.end()) {
                    return iter->second;
                }
                T created = Creator((EntryType) _raw_pool[index]);
                _pool.insert(std::make_pair(index, created));
                return created;
            }
        };
    }

    class RuntimeConstantPool {
    private:
        ClassLoader *_class_loader;

    public:
        explicit RuntimeConstantPool(InstanceKlass *instanceKlass);

        void attachConstantPool(cp_info **pool) {
        }
    };
}
