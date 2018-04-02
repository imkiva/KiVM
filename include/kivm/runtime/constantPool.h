//
// Created by kiva on 2018/3/31.
//
#pragma once

#include <kivm/classfile/constantPool.h>
#include <kivm/oop/oopfwd.h>
#include <kivm/oop/instanceKlass.h>
#include <kivm/classLoader.h>
#include <kivm/method.h>
#include <kivm/field.h>
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

            T findOrNew(RuntimeConstantPool *rt, int index) {
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

        struct CreatorHelper {
            static const String &
            getUtf8(cp_info **pool, int utf8Index) {
                auto *utf8 = (CONSTANT_Utf8_info *) pool[utf8Index];
                return utf8->get_constant();
            }

            static const std::pair<const String &, const String &> &
            getNameAndType(cp_info **pool, int nameAndTypeIndex) {
                auto nameAndType = (CONSTANT_NameAndType_info *) pool[nameAndTypeIndex];
                return std::make_pair(getUtf8(pool, nameAndType->name_index),
                                      getUtf8(pool, nameAndType->descriptor_index));
            }
        };

        struct ClassCreator {
            Klass *operator()(RuntimeConstantPool *rt, cp_info **pool, int index) {
                auto classInfo = (CONSTANT_Class_info *) pool[index];
                return BootstrapClassLoader::get()->loadClass(
                    CreatorHelper::getUtf8(pool, classInfo->name_index));
            }
        };

        struct StringCreator {
            instanceOop operator()(RuntimeConstantPool *rt, cp_info **pool, int index) {
                auto classInfo = (CONSTANT_String_info *) pool[index];
                return java::lang::String::from(
                    CreatorHelper::getUtf8(pool, classInfo->string_index));
            }
        };

        struct MethodCreator {
#pragma clang diagnostic push
#pragma ide diagnostic ignored "OCDFAInspection"

            Method *operator()(RuntimeConstantPool *rt, cp_info **pool, int index) {
                auto methodRef = (CONSTANT_Methodref_info *) pool[index];
                Klass *klass = rt->get_class(methodRef->class_index);
                if (klass->getClassType() == ClassType::INSTANCE_CLASS) {
                    auto instanceKlass = (InstanceKlass *) klass;
                    const auto &nameAndType = CreatorHelper::getNameAndType(
                        pool, methodRef->name_and_type_index);
                }
                return nullptr;
            }

#pragma clang diagnostic pop
        };

        struct FieldCreator {
            Field *operator()(RuntimeConstantPool *rt, cp_info **pool, int index) {
                return nullptr;
            }
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

        instanceOop get_string(int stringIndex) {
            assert(this->_raw_pool != nullptr);
            return _string_pool.findOrNew(this, stringIndex);
        }

        inline Method *get_method(int methodIndex) {
            assert(this->_raw_pool != nullptr);
            return _method_pool.findOrNew(this, methodIndex);
        }

        Field *get_field(int fieldIndex) {
            assert(this->_raw_pool != nullptr);
            return _field_pool.findOrNew(this, fieldIndex);
        }
    };
}
