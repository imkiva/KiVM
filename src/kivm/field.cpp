//
// Created by kiva on 2018/2/28.
//
#include <kivm/field.h>
#include <kivm/classFile.h>

namespace kivm {

    Field::Field(InstanceKlass *clazz, field_info *field_info) {
        this->_linked = false;
        this->_klass = clazz;
        this->_field_info = field_info;
        this->_constant_attribute = nullptr;
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
}
