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
}

