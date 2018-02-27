//
// Created by kiva on 2018/2/27.
//

#include <kivm/method.h>
#include <kivm/instanceKlass.h>
#include <shared/lock.h>

#include <sstream>

namespace kivm {
    static Lock &get_method_pool_lock() {
        static Lock _method_pool_lock;
        return _method_pool_lock;
    }

    void MethodPool::add(Method *method) {
        LockGuard guard(get_method_pool_lock());
        entries_internal().push_back(method);
    }

    std::list<Method *> &MethodPool::entries_internal() {
        static std::list<Method *> _entries;
        return _entries;
    }

    const std::list<Method *> &MethodPool::entries() {
        return entries_internal();
    }

    bool Method::is_same(const Method *lhs, const Method *rhs) {
        return lhs != nullptr && rhs != nullptr
               && lhs->get_name() == rhs->get_name()
               && lhs->get_descriptor() == rhs->get_descriptor();
    }

    const String &Method::make_identity(const Method *m) {
        std::wstringstream ss;
        ss << m->get_name() << L" " << m->get_descriptor();
        return ss.str();
    }

    Method::Method(InstanceKlass *clazz, method_info *method_info) {
        this->_linked = false;
        this->_klass = clazz;
        this->_method_info = method_info;
    }

    bool Method::is_pc_in_method(const u1 *pc) {
        return _code_attr != nullptr
               && _code_attr->code_length > 0
               && _code_attr->code != nullptr
               && (pc - _code_attr->code) < _code_attr->code_length;
    }

    void Method::link_method(cp_info **pool) {
        if (_linked) {
            return;
        }

        this->_access_flag = _method_info->access_flags;
        auto *name_info = require_constant<CONSTANT_Utf8_info>(pool, _method_info->name_index);
        auto *desc_info = require_constant<CONSTANT_Utf8_info>(pool, _method_info->descriptor_index);
        this->_name = name_info->get_constant();
        this->_descriptor = desc_info->get_constant();
        link_attributes(pool);
        _linked = true;
    }

    void Method::link_attributes(cp_info **pool) {
        for (int i = 0; i < _method_info->attributes_count; ++i) {
            attribute_info *attr = _method_info->attributes[i];

            switch (AttributeParser::to_attribute_tag(attr->attribute_name_index, pool)) {
                case ATTRIBUTE_Code: {
                    link_code_attribute(pool, (Code_attribute *) attr);
                    break;
                }
                case ATTRIBUTE_Exceptions: {
                    link_exception_attribute(pool, (Exceptions_attribute *) attr);
                    break;
                }
                case ATTRIBUTE_Signature: {
                    auto *sig_attr = (Signature_attribute *) attr;
                    auto *utf8 = require_constant<CONSTANT_Utf8_info>(pool, sig_attr->signature_index);
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

    void Method::link_exception_attribute(cp_info **pool, Exceptions_attribute *attr) {
        for (int i = 0; i < attr->number_of_exceptions; ++i) {
            u2 exception_index = attr->exception_index_table[i];
            auto *class_info = require_constant<CONSTANT_Class_info>(pool, exception_index);
            auto *utf8_info = require_constant<CONSTANT_Utf8_info>(pool, class_info->name_index);
            Klass *loaded = ClassLoader::require_class(get_class()->get_class_loader(),
                                                       utf8_info->get_constant());
            if (loaded->get_type() != ClassType::INSTANCE_CLASS) {
                // TODO: throw VerifyError
                assert(false);
                continue;
            }

            auto *exception_class = (InstanceKlass *) loaded;
            _throws.push_back(exception_class);
        }
    }

    void Method::link_code_attribute(cp_info **pool, Code_attribute *attr) {
        _code_attr = attr;
        // check exception handlers
        for (int i = 0; i < attr->exception_table_length; ++i) {
            u1 *code_base = attr->code;
            if (is_pc_in_method(code_base + attr->exception_table[i].start_pc)
                && is_pc_in_method(code_base + attr->exception_table[i].end_pc)
                && is_pc_in_method(code_base + attr->exception_table[i].handler_pc)) {
                continue;
            }
            // TODO: throw VerifyError
            assert(false);
        }

        // link attributes
        for (int i = 0; i < attr->attributes_count; ++i) {
            attribute_info *sub_attr = attr->attributes[i];
            switch (AttributeParser::to_attribute_tag(sub_attr->attribute_name_index, pool)) {
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
    }
}
