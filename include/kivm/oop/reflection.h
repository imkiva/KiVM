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

    inline ValueType primitiveTypeToValueType(char c) {
        switch (c) {
            case 'B':    // byte
            case 'Z':    // boolean
            case 'S':    // short
            case 'C':    // char
            case 'I':    // int
                return ValueType::INT;
            case 'J':    // long
                return ValueType::LONG;
            case 'F':    // float
                return ValueType::FLOAT;
            case 'D':    // double
                return ValueType::DOUBLE;
            case 'V':    // void
                return ValueType::VOID;
            default:
                PANIC("primitive type required");
                break;
        }
    }

    inline ValueType primitiveTypeToValueTypeNoWrap(char c) {
        switch (c) {
            case 'B':    // byte
                return ValueType::BYTE;
            case 'Z':    // boolean
                return ValueType::BOOLEAN;
            case 'S':    // short
                return ValueType::SHORT;
            case 'C':    // char
                return ValueType::CHAR;
            case 'I':    // int
                return ValueType::INT;
            case 'J':    // long
                return ValueType::LONG;
            case 'F':    // float
                return ValueType::FLOAT;
            case 'D':    // double
                return ValueType::DOUBLE;
            case 'V':    // void
                return ValueType::VOID;
            default:
                PANIC("primitive type required");
                break;
        }
    }

    inline char valueTypeToPrimitiveType(ValueType v) {
        switch (v) {
            case ValueType::BOOLEAN:
                return 'Z';
            case ValueType::BYTE:
                return 'B';
            case ValueType::CHAR:
                return 'C';
            case ValueType::SHORT:
                return 'S';
            case ValueType::INT:
                return 'I';
            case ValueType::FLOAT:
                return 'F';
            case ValueType::LONG:
                return 'J';
            case ValueType::DOUBLE:
                return 'D';
            case ValueType::VOID:
                return 'V';
            default:
                PANIC("primitive type required");
        }
    }

    inline String valueTypeToPrimitiveTypeDesc(ValueType v) {
        switch (v) {
            case ValueType::BOOLEAN:
                return "Z";
            case ValueType::BYTE:
                return "B";
            case ValueType::CHAR:
                return "C";
            case ValueType::SHORT:
                return "S";
            case ValueType::INT:
                return "I";
            case ValueType::FLOAT:
                return "F";
            case ValueType::LONG:
                return "J";
            case ValueType::DOUBLE:
                return "D";
            case ValueType::VOID:
                return "V";
            default:
                PANIC("primitive type required");
        }
    }

    inline String valueTypeToPrimitiveTypeName(ValueType v) {
        switch (v) {
            case ValueType::BOOLEAN:
                return "boolean";
            case ValueType::BYTE:
                return "byte";
            case ValueType::CHAR:
                return "char";
            case ValueType::SHORT:
                return "short";
            case ValueType::INT:
                return "int";
            case ValueType::FLOAT:
                return "float";
            case ValueType::LONG:
                return "long";
            case ValueType::DOUBLE:
                return "double";
            case ValueType::VOID:
                return "void";
            default:
                PANIC("primitive type required");
        }
    }

    mirrorOop getClassFromConstructor(instanceOop ctorOop);

    jint getSlotFromConstructor(instanceOop ctorOop);

    instanceOop newJavaFieldObject(FieldID *fieldID);

    instanceOop newJavaMethodObject(MethodID *method);

    instanceOop newJavaConstructorObject(MethodID *method);
}
