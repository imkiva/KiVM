//
// Created by kiva on 2018/2/28.
//
#pragma once

#include <kivm/oop/oop.h>

namespace kivm {
    template<typename T>
    struct PrimitiveHelper {
        constexpr static ValueType getValueType() = delete;
    };

    template<>
    struct PrimitiveHelper<jint> {
        constexpr static ValueType getValueType() {
            return ValueType::INT;
        }
    };

    template<>
    struct PrimitiveHelper<jlong> {
        constexpr static ValueType getValueType() {
            return ValueType::LONG;
        };
    };

    template<>
    struct PrimitiveHelper<long> {
        constexpr static ValueType getValueType() {
            return ValueType::LONG;
        };
    };

    template<>
    struct PrimitiveHelper<jfloat> {
        constexpr static ValueType getValueType() {
            return ValueType::FLOAT;
        };
    };

    template<>
    struct PrimitiveHelper<jdouble> {
        constexpr static ValueType getValueType() {
            return ValueType::DOUBLE;
        }
    };

    template<typename T>
    class primitiveOopDesc : public oopDesc {
    private:
        T _value;

    public:
        explicit primitiveOopDesc(T value)
            : oopDesc(nullptr, oopType::PRIMITIVE_OOP), _value(value) {
        }

        ValueType getPrimitiveType() const {
            return PrimitiveHelper<T>::getValueType();
        }

        T getValue() const {
            return _value;
        }
    };

    class intOopDesc : public primitiveOopDesc<jint> {
    public:
        explicit intOopDesc(jint value);
    };

    class longOopDesc : public primitiveOopDesc<jlong> {
    public:
        explicit longOopDesc(jlong value);
    };

    class floatOopDesc : public primitiveOopDesc<jfloat> {
    public:
        explicit floatOopDesc(jfloat value);
    };

    class doubleOopDesc : public primitiveOopDesc<jdouble> {
    public:
        explicit doubleOopDesc(jdouble value);
    };
}
