//
// Created by kiva on 2018/3/31.
//
#pragma once

#include <kivm/classfile/constantPool.h>
#include <kivm/oop/oopfwd.h>
#include <kivm/classLoader.h>
#include <kivm/native/java_lang_String.h>
#include <unordered_map>

namespace kivm {
    class Klass;

    class InstanceKlass;

    namespace pools {
        template<typename T, typename Creator, int CONSTANT_TAG>
        class ConstantTable {
        private:
            cp_info **_raw_pool = nullptr;

            // constant-pool-index -> constant
            std::unordered_map<int, T> _pool;

        public:
            void setRawPool(cp_info **pool) {
                this->_raw_pool = pool;
            }

            T findOrNew(int index) {
                if (_raw_pool[index]->tag != CONSTANT_TAG) {
                    PANIC("Accessing an incompatible constant entry");
                }
                const auto &iter = _pool.find(index);
                if (iter != _pool.end()) {
                    return iter->second;
                }
                T created = Creator(_raw_pool, index);
                _pool.insert(std::make_pair(index, created));
                return created;
            }
        };

        struct ClassCreator {
            Klass *operator()(cp_info **pool, int index) {
                auto classInfo = (CONSTANT_Class_info *) pool[index];
                auto *utf8 = (CONSTANT_Utf8_info *) pool[classInfo->name_index];
                return BootstrapClassLoader::get()->loadClass(utf8->get_constant());
            }
        };

        struct StringCreator {
            instanceOop operator()(cp_info **pool, int index) {
                auto classInfo = (CONSTANT_String_info *) pool[index];
                auto *utf8 = (CONSTANT_Utf8_info *) pool[classInfo->string_index];
                return java::lang::String::from(utf8->get_constant());
            }
        };

        using ClassTable = ConstantTable<Klass *, ClassCreator, CONSTANT_Class>;
        using StringTable = ConstantTable<instanceOop, StringCreator, CONSTANT_String>;
    }

    class RuntimeConstantPool {
    private:
        ClassLoader *_class_loader;
        pools::ClassTable _class_table;
        pools::StringTable _string_table;

    public:
        explicit RuntimeConstantPool(InstanceKlass *instanceKlass);

        void attachConstantPool(cp_info **pool) {
            _class_table.setRawPool(pool);
            _string_table.setRawPool(pool);
        }
    };
}
