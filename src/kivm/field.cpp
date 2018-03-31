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
        getEntriesInternal().push_back(method);
    }

    std::list<Field *> &FieldPool::getEntriesInternal() {
        static std::list<Field *> _entries;
        return _entries;
    }

    const std::list<Field *> &FieldPool::getEntries() {
        return getEntriesInternal();
    }

    bool Field::isSame(const Field *lhs, const Field *rhs) {
        return lhs != nullptr && rhs != nullptr
               && lhs->getName() == rhs->getName()
               && lhs->getDescriptor() == rhs->getDescriptor();
    }

    String Field::makeIdentity(InstanceKlass *belongTo, const Field *f) {
        std::wstringstream ss;
        ss << belongTo->getName() << L" " << f->getName() << L" " << f->getDescriptor();
        return ss.str();
    }

    Field::Field(InstanceKlass *clazz, field_info *field_info) {
        this->_linked = false;
        this->_klass = clazz;
        this->_field_info = field_info;
        this->_constant_attribute = nullptr;
        this->_value_class_type = nullptr;
    }

    void Field::linkField(cp_info **pool) {
        if (_linked) {
            return;
        }
        this->_access_flag = _field_info->access_flags;
        auto *name_info = requireConstant<CONSTANT_Utf8_info>(pool, _field_info->name_index);
        auto *desc_info = requireConstant<CONSTANT_Utf8_info>(pool, _field_info->descriptor_index);
        this->_name = name_info->get_constant();
        this->_descriptor = desc_info->get_constant();
        linkAttributes(pool);
        linkValueType();
        this->_linked = true;
    }

    void Field::linkAttributes(cp_info **pool) {
        for (int i = 0; i < _field_info->attributes_count; ++i) {
            attribute_info *attr = _field_info->attributes[i];

            switch (AttributeParser::toAttributeTag(attr->attribute_name_index, pool)) {
                case ATTRIBUTE_ConstantValue: {
                    _constant_attribute = (ConstantValue_attribute *) (attr);
                    break;
                }
                case ATTRIBUTE_Signature: {
                    auto *sig_attr = (Signature_attribute *) attr;
                    auto *utf8 = requireConstant<CONSTANT_Utf8_info>(pool, sig_attr->signature_index);
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

    void Field::linkValueType() {
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
                _value_class_type = ClassLoader::requireClass(getClass()->getClassLoader(),
                                                              class_name);
                break;
            }
            case L'[': {
                _value_type = ValueType::ARRAY;
                _value_class_type = ClassLoader::requireClass(getClass()->getClassLoader(),
                                                              _descriptor);
                break;
            }
            default:
                // TODO: throw VerifyError
                assert(false);
        }
    }
}
