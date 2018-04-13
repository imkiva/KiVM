//
// Created by kiva on 2018/2/27.
//

#include <kivm/method.h>
#include <kivm/oop/instanceKlass.h>
#include <shared/lock.h>

#include <sstream>

namespace kivm {
    static Lock &get_method_pool_lock() {
        static Lock _method_pool_lock;
        return _method_pool_lock;
    }

    void MethodPool::add(Method *method) {
        LockGuard guard(get_method_pool_lock());
        getEntriesInternal().push_back(method);
    }

    std::list<Method *> &MethodPool::getEntriesInternal() {
        static std::list<Method *> _entries;
        return _entries;
    }

    const std::list<Method *> &MethodPool::getEntries() {
        return getEntriesInternal();
    }

    bool Method::isSame(const Method *lhs, const Method *rhs) {
        return lhs != nullptr && rhs != nullptr
               && lhs->getName() == rhs->getName()
               && lhs->getDescriptor() == rhs->getDescriptor();
    }

    String Method::makeIdentity(const Method *m) {
        std::wstringstream ss;
        ss << m->getName() << L" " << m->getDescriptor();
        return ss.str();
    }

    Method::Method(InstanceKlass *clazz, method_info *method_info) {
        this->_linked = false;
        this->_klass = clazz;
        this->_method_info = method_info;
        this->_code_attr = nullptr;
        this->_exception_attr = nullptr;
        this->_argument_value_types_resolved = false;
        this->_return_type_resolved = false;
    }

    bool Method::isPcCorrect(u4 pc) {
        return _code_attr != nullptr
               && _code_attr->code_length > 0
               && pc < _code_attr->code_length;
    }

    void Method::linkMethod(cp_info **pool) {
        if (_linked) {
            return;
        }

        this->_access_flag = _method_info->access_flags;
        auto *name_info = requireConstant<CONSTANT_Utf8_info>(pool, _method_info->name_index);
        auto *desc_info = requireConstant<CONSTANT_Utf8_info>(pool, _method_info->descriptor_index);
        this->_name = name_info->get_constant();
        this->_descriptor = desc_info->get_constant();
        linkAttributes(pool);
        _linked = true;
    }

    void Method::linkAttributes(cp_info **pool) {
        for (int i = 0; i < _method_info->attributes_count; ++i) {
            attribute_info *attr = _method_info->attributes[i];

            switch (AttributeParser::toAttributeTag(attr->attribute_name_index, pool)) {
                case ATTRIBUTE_Code: {
                    linkCodeAttribute(pool, (Code_attribute *) attr);
                    break;
                }
                case ATTRIBUTE_Exceptions: {
                    linkExceptionAttribute(pool, (Exceptions_attribute *) attr);
                    break;
                }
                case ATTRIBUTE_Signature: {
                    auto *sig_attr = (Signature_attribute *) attr;
                    auto *utf8 = requireConstant<CONSTANT_Utf8_info>(pool, sig_attr->signature_index);
                    _signature = utf8->get_constant();
                    break;
                }
                case ATTRIBUTE_RuntimeVisibleAnnotations:
                case ATTRIBUTE_RuntimeVisibleParameterAnnotations:
                case ATTRIBUTE_RuntimeVisibleTypeAnnotations:
                case ATTRIBUTE_AnnotationDefault:
                default: {
                    break;
                }
            }
        }
    }

    void Method::linkExceptionAttribute(cp_info **pool, Exceptions_attribute *attr) {
        for (int i = 0; i < attr->number_of_exceptions; ++i) {
            u2 exception_index = attr->exception_index_table[i];
            auto *class_info = requireConstant<CONSTANT_Class_info>(pool, exception_index);
            auto *utf8_info = requireConstant<CONSTANT_Utf8_info>(pool, class_info->name_index);
            Klass *loaded = ClassLoader::requireClass(getClass()->getClassLoader(),
                                                      utf8_info->get_constant());
            if (loaded->getClassType() != ClassType::INSTANCE_CLASS) {
                // TODO: throw VerifyError
                assert(false);
                continue;
            }

            auto *exception_class = (InstanceKlass *) loaded;
            _throws.push_back(exception_class);
        }
    }

    void Method::linkCodeAttribute(cp_info **pool, Code_attribute *attr) {
        _code_attr = attr;
        // check exception handlers
        for (int i = 0; i < attr->exception_table_length; ++i) {
            if (isPcCorrect(attr->exception_table[i].start_pc)
                && isPcCorrect(attr->exception_table[i].end_pc)
                && isPcCorrect(attr->exception_table[i].handler_pc)) {
                continue;
            }
            // TODO: throw VerifyError
            assert(false);
        }

        // link attributes
        for (int i = 0; i < attr->attributes_count; ++i) {
            attribute_info *sub_attr = attr->attributes[i];
            switch (AttributeParser::toAttributeTag(sub_attr->attribute_name_index, pool)) {
                case ATTRIBUTE_LineNumberTable: {
                    auto *line_attr = (LineNumberTable_attribute *) sub_attr;
                    for (int j = 0; j < line_attr->line_number_table_length; ++j) {
                        _line_number_table[line_attr->line_number_table[j].start_pc]
                            = line_attr->line_number_table[j].line_number;
                    }
                    break;
                }
                case ATTRIBUTE_RuntimeVisibleTypeAnnotations:
                case ATTRIBUTE_StackMapTable:
                case ATTRIBUTE_LocalVariableTable:
                case ATTRIBUTE_LocalVariableTypeTable:
                case ATTRIBUTE_RuntimeInvisibleTypeAnnotations:
                default:
                    // TODO
                    break;
            }
        }

        _code_blob.init(_code_attr->code, _code_attr->code_length);
    }

    const std::list<ValueType> &Method::getArgumentValueTypes() {
        if (_argument_value_types_resolved) {
            return _argument_value_types;
        }
        _argument_value_types_resolved = true;

        const String &desc = getDescriptor();
        for (int i = 0; i < desc.size(); ++i) {
            wchar_t ch = desc[i];
            switch (ch) {
                case L'B':    // byte
                case L'Z':    // boolean
                case L'S':    // short
                case L'C':    // char
                case L'I':    // int
                case L'J':    // long
                case L'F':    // float
                case L'D':    // double
                    _argument_value_types.push_back(primitiveTypeToValueType(ch));
                    break;

                case L'L':
                    while (desc[i] != ';') {
                        ++i;
                    }
                    _argument_value_types.push_back(ValueType::OBJECT);
                    break;

                case L'(':
                    break;

                case L')':
                    return _argument_value_types;

                default:
                    PANIC("Unrecognized char %c in descriptor", ch);
            }
        }
        return _argument_value_types;
    }

    ValueType Method::getReturnType() {
        if (_return_type_resolved) {
            return _return_type;
        }
        _return_type_resolved = true;

        const String &returnTypeDesc = getDescriptor().substr(getDescriptor().find_first_of(L')') + 1);
        int i = 0;
        wchar_t ch = returnTypeDesc[i];
        switch (ch) {
            case L'B':    // byte
            case L'Z':    // boolean
            case L'S':    // short
            case L'C':    // char
            case L'I':    // int
            case L'J':    // long
            case L'F':    // float
            case L'D':    // double
            case L'V':    // void
                _return_type = primitiveTypeToValueType(ch);
                break;

            case L'L':
                while (returnTypeDesc[i] != ';') {
                    ++i;
                }
                _return_type = ValueType::OBJECT;
                break;

            default:
                PANIC("Unrecognized char %c in descriptor", ch);
        }
        return _return_type;
    }
}
