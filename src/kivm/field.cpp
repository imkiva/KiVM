//
// Created by kiva on 2018/2/28.
//
#include <kivm/field.h>
#include <kivm/classfile/classFile.h>
#include <kivm/oop/instanceKlass.h>
#include <shared/lock.h>

#include <sstream>

namespace kivm {
    static Lock &get_method_pool_lock() {
        static Lock _method_pool_lock;
        return _method_pool_lock;
    }

    void FieldPool::add(Field *method) {
        LockGuard guard(get_method_pool_lock());
        entries_internal().push_back(method);
    }

    std::list<Field *> &FieldPool::entries_internal() {
        static std::list<Field *> _entries;
        return _entries;
    }

    const std::list<Field *> &FieldPool::entries() {
        return entries_internal();
    }

    bool Field::is_same(const Field *lhs, const Field *rhs) {
        return lhs != nullptr && rhs != nullptr
               && lhs->get_name() == rhs->get_name()
               && lhs->get_descriptor() == rhs->get_descriptor();
    }

    String Field::make_identity(const Field *f) {
        std::wstringstream ss;
        ss << f->get_name() << L" " << f->get_descriptor();
        return ss.str();
    }

    Field::Field(InstanceKlass *clazz, field_info *field_info) {
        this->_linked = false;
        this->_klass = clazz;
        this->_field_info = field_info;
        this->_constant_attribute = nullptr;
        this->_value_class_type = nullptr;
    }

    void Field::link_field(cp_info **pool) {
        if (_linked) {
            return;
        }
        this->_access_flag = _field_info->access_flags;
        auto *name_info = require_constant<CONSTANT_Utf8_info>(pool, _field_info->name_index);
        auto *desc_info = require_constant<CONSTANT_Utf8_info>(pool, _field_info->descriptor_index);
        this->_name = name_info->get_constant();
        this->_descriptor = desc_info->get_constant();
        link_attributes(pool);
        link_value_type();
        this->_linked = true;
    }

    void Field::link_attributes(cp_info **pool) {
        for (int i = 0; i < _field_info->attributes_count; ++i) {
            attribute_info *attr = _field_info->attributes[i];

            switch (AttributeParser::to_attribute_tag(attr->attribute_name_index, pool)) {
                case ATTRIBUTE_ConstantValue: {
                    _constant_attribute = (ConstantValue_attribute *) (attr);
                    break;
                }
                case ATTRIBUTE_Signature: {
                    auto *sig_attr = (Signature_attribute *) attr;
                    auto *utf8 = require_constant<CONSTANT_Utf8_info>(pool, sig_attr->signature_index);
                    _signature = utf8->get_constant();
                    break;
                }

                case ATTRIBUTE_RuntimeVisibleAnnotations:
                case ATTRIBUTE_RuntimeVisibleTypeAnnotations:
                default:
                    // TODO
                    break;
            }
        }
    }

    void Field::link_value_type() {
        switch (_descriptor[0]) {
            case L'Z':
                _value_type = ValueType::BOOLEAN;
                break;
            case L'B':
                _value_type = ValueType::BYTE;
                break;
            case L'C':
                _value_type = ValueType::CHAR;
                break;
            case L'S':
                _value_type = ValueType::SHORT;
                break;
            case L'I':
                _value_type = ValueType::INT;
                break;
            case L'F':
                _value_type = ValueType::FLOAT;
                break;
            case L'J':
                _value_type = ValueType::LONG;
                break;
            case L'D':
                _value_type = ValueType::DOUBLE;
                break;
            case L'L': {
                _value_type = ValueType::OBJECT;
                const String &class_name = _descriptor.substr(1, _descriptor.size() - 2);
                _value_class_type = ClassLoader::require_class(get_class()->get_class_loader(),
                                                               class_name);
                break;
            }
            case L'[': {
                _value_type = ValueType::ARRAY;
                _value_class_type = ClassLoader::require_class(get_class()->get_class_loader(),
                                                               _descriptor);
                break;
            }
            default:
                // TODO: throw VerifyError
                assert(false);
        }
    }
}
