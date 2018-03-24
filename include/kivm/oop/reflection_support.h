//
// Created by kiva on 2018/3/24.
//
#pragma once

#include <kivm/field.h>

namespace kivm {
    struct FieldID {
        int _offset;
        Field *_field;

        FieldID(int _offset, Field *_field) : _offset(_offset), _field(_field) {}
    };
}
