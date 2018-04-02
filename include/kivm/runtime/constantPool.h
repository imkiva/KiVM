//
// Created by kiva on 2018/3/31.
//
#pragma once

#include <kivm/oop/oopfwd.h>
#include <kivm/classfile/constantPool.h>
#include <kivm/native/java_lang_String.h>
#include <kivm/classLoader.h>
#include <kivm/method.h>
#include <kivm/field.h>
#include <unordered_map>

namespace kivm {
    class Klass;

    class InstanceKlass;

    class RuntimeConstantPool;

    namespace pools {
        template<typename T, typename Creator, int CONSTANT_TAG>
        class ConstantTable {
        private:
            cp_info **_raw_pool = nullptr;

            // constant-pool-index -> constant
            std::unordered_map<int, T> _pool;

        public:
            inline void setRawPool(cp_info **pool) {
                this->_raw_pool = pool;
            }

            inline T findOrNew(RuntimeConstantPool *rt, int index) {
                if (_raw_pool[index]->tag != CONSTANT_TAG) {
                    PANIC("Accessing an incompatible constant entry");
                }
                const auto &iter = _pool.find(index);
                if (iter != _pool.end()) {
                    return iter->second;
                }
                T created = Creator()(rt, _raw_pool, index);
                _pool.insert(std::make_pair(index, created));
                return created;
            }
        };

        struct ClassCreator {
            Klass *operator()(RuntimeConstantPool *rt, cp_info **pool, int index);
        };

        struct StringCreator {
            instanceOop operator()(RuntimeConstantPool *rt, cp_info **pool, int index);
        };

        struct MethodCreator {
            Method *operator()(RuntimeConstantPool *rt, cp_info **pool, int index);
        };

        struct FieldCreator {
            Field *operator()(RuntimeConstantPool *rt, cp_info **pool, int index);
        };

        using ClassPool = ConstantTable<Klass *, ClassCreator, CONSTANT_Class>;
        using StringPool = ConstantTable<instanceOop, StringCreator, CONSTANT_String>;
        using MethodPool = ConstantTable<Method *, MethodCreator, CONSTANT_Methodref>;
        using FieldPool = ConstantTable<Field *, FieldCreator, CONSTANT_Fieldref>;
    }

    class RuntimeConstantPool {
    private:
        ClassLoader *_class_loader;
        cp_info **_raw_pool;
        pools::ClassPool _class_pool;
        pools::StringPool _string_pool;
        pools::MethodPool _method_pool;
        pools::FieldPool _field_pool;

    public:
        explicit RuntimeConstantPool(InstanceKlass *instanceKlass);

        void attachConstantPool(cp_info **pool) {
            this->_raw_pool = pool;
            _class_pool.setRawPool(pool);
            _string_pool.setRawPool(pool);
        }

        inline Klass *get_class(int classIndex) {
            assert(this->_raw_pool != nullptr);
            return _class_pool.findOrNew(this, classIndex);
        }

        inline instanceOop get_string(int stringIndex) {
            assert(this->_raw_pool != nullptr);
            return _string_pool.findOrNew(this, stringIndex);
        }

        inline Method *get_method(int methodIndex) {
            assert(this->_raw_pool != nullptr);
            return _method_pool.findOrNew(this, methodIndex);
        }

        inline Field *get_field(int fieldIndex) {
            assert(this->_raw_pool != nullptr);
            return _field_pool.findOrNew(this, fieldIndex);
        }
    };
}
