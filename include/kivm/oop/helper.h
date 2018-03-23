//
// Created by kiva on 2018/3/23.
//
#pragma once

#include <vector>
#include <kivm/oop/primitiveOop.h>
#include <kivm/field.h>

namespace kivm {
    inline void helper_init_field(std::vector<oop> &values, Field *field) {
        switch (field->get_value_type()) {
            case ValueType::INT:
                values.push_back(new intOopDesc(0));
                break;
            case ValueType::LONG:
                values.push_back(new longOopDesc(0L));
                break;
            case ValueType::FLOAT:
                values.push_back(new floatOopDesc(0.0f));
                break;
            case ValueType::DOUBLE:
                values.push_back(new doubleOopDesc(0.0));
                break;
            default:
                values.push_back(nullptr);
                break;
        }
    }

    inline bool helper_init_static_final_field(std::vector<oop> &values,
                                               cp_info **pool,
                                               Field *field) {
        ConstantValue_attribute *attr = field->get_constant_attribute();
        if (attr != nullptr) {
            cp_info *constant_info = pool[attr->constant_index];
            switch (constant_info->tag) {
                case CONSTANT_Long: {
                    auto *info = (CONSTANT_Long_info *) constant_info;
                    values.push_back(new longOopDesc(info->get_constant()));
                    break;
                }
                case CONSTANT_Float: {
                    auto *info = (CONSTANT_Float_info *) constant_info;
                    values.push_back(new floatOopDesc(info->get_constant()));
                    break;
                }
                case CONSTANT_Double: {
                    auto *info = (CONSTANT_Double_info *) constant_info;
                    values.push_back(new doubleOopDesc(info->get_constant()));
                    break;
                }
                case CONSTANT_Integer: {
                    auto *info = (CONSTANT_Integer_info *) constant_info;
                    values.push_back(new intOopDesc(info->get_constant()));
                    break;
                }
                case CONSTANT_String: {
                    auto *info = (CONSTANT_String_info *) constant_info;
                    // TODO: init string constants
                    break;
                }
                default: {
                    assert(false);
                }
            }
        }
        return true;
    }
}

