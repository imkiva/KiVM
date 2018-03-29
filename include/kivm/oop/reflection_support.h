//
// Created by kiva on 2018/3/24.
//
#pragma once

#include <kivm/oop/oopfwd.h>
#include <kivm/field.h>

namespace kivm {
    struct FieldID {
        int _offset;
        Field *_field;

        FieldID(int _offset, Field *_field) : _offset(_offset), _field(_field) {}
    };

    inline ValueType primitive_type_to_value_type(wchar_t c) {
        switch (c) {
            case L'B':    // byte
            case L'Z':    // boolean
            case L'S':    // short
            case L'C':    // char
            case L'I':    // int
                return ValueType::INT;
            case L'J':
                return ValueType::LONG;
            case L'F':    // float
                return ValueType::FLOAT;
            case L'D':    // double
                return ValueType::DOUBLE;
            case L'V':    // void
                return ValueType::VOID;
            default:
                PANIC("primitive type required");
                break;
        }
    }

    inline wchar_t value_type_to_primitive_type(ValueType v) {
        switch (v) {
            case ValueType::BOOLEAN:
                return L'Z';
            case ValueType::BYTE:
                return L'B';
            case ValueType::CHAR:
                return L'C';
            case ValueType::SHORT:
                return L'S';
            case ValueType::INT:
                return L'I';
            case ValueType::FLOAT:
                return L'F';
            case ValueType::LONG:
                return L'J';
            case ValueType::DOUBLE:
                return L'D';
            case ValueType::VOID:
                return L'V';
            default:
                PANIC("primitive type required");
        }
    }
}
