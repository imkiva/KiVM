//
// Created by kiva on 2018/3/24.
//
#pragma once

#include <kivm/oop/oopfwd.h>
#include <kivm/oop/field.h>
#include <kivm/oop/method.h>

namespace kivm {
    struct FieldID {
        int _offset;
        Field *_field;

        FieldID(int _offset, Field *_field)
            : _offset(_offset), _field(_field) {
        }
    };

    struct MethodID {
        int _offset;
        Method *_method;

        MethodID(int _offset, Method *_method)
            : _offset(_offset), _method(_method) {
        }
    };

    inline ValueType primitiveTypeToValueType(wchar_t c) {
        switch (c) {
            case L'B':    // byte
            case L'Z':    // boolean
            case L'S':    // short
            case L'C':    // char
            case L'I':    // int
                return ValueType::INT;
            case L'J':    // long
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

    inline ValueType primitiveTypeToValueTypeNoWrap(wchar_t c) {
        switch (c) {
            case L'B':    // byte
                return ValueType::BYTE;
            case L'Z':    // boolean
                return ValueType::BOOLEAN;
            case L'S':    // short
                return ValueType::SHORT;
            case L'C':    // char
                return ValueType::CHAR;
            case L'I':    // int
                return ValueType::INT;
            case L'J':    // long
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

    inline wchar_t valueTypeToPrimitiveType(ValueType v) {
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

    inline String valueTypeToPrimitiveTypeDesc(ValueType v) {
        switch (v) {
            case ValueType::BOOLEAN:
                return L"Z";
            case ValueType::BYTE:
                return L"B";
            case ValueType::CHAR:
                return L"C";
            case ValueType::SHORT:
                return L"S";
            case ValueType::INT:
                return L"I";
            case ValueType::FLOAT:
                return L"F";
            case ValueType::LONG:
                return L"J";
            case ValueType::DOUBLE:
                return L"D";
            case ValueType::VOID:
                return L"V";
            default:
                PANIC("primitive type required");
        }
    }

    inline String valueTypeToPrimitiveTypeName(ValueType v) {
        switch (v) {
            case ValueType::BOOLEAN:
                return L"boolean";
            case ValueType::BYTE:
                return L"byte";
            case ValueType::CHAR:
                return L"char";
            case ValueType::SHORT:
                return L"short";
            case ValueType::INT:
                return L"int";
            case ValueType::FLOAT:
                return L"float";
            case ValueType::LONG:
                return L"long";
            case ValueType::DOUBLE:
                return L"double";
            case ValueType::VOID:
                return L"void";
            default:
                PANIC("primitive type required");
        }
    }

    instanceOop newJavaFieldObject(FieldID *fieldID);

    instanceOop newJavaMethodObject(MethodID *method);

    instanceOop newJavaConstructorObject(MethodID *method);
}
