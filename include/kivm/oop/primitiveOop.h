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
    struct PrimitiveHelper<int> {
        constexpr static ValueType getValueType() {
            return ValueType::INT;
        }
    };

    template<>
    struct PrimitiveHelper<long> {
        constexpr static ValueType getValueType() {
            return ValueType::LONG;
        };
    };

    template<>
    struct PrimitiveHelper<float> {
        constexpr static ValueType getValueType() {
            return ValueType::FLOAT;
        };
    };

    template<>
    struct PrimitiveHelper<double> {
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

    class intOopDesc : public primitiveOopDesc<int> {
    public:
        explicit intOopDesc(int value);
    };

    class longOopDesc : public primitiveOopDesc<long> {
    public:
        explicit longOopDesc(long value);
    };

    class floatOopDesc : public primitiveOopDesc<float> {
    public:
        explicit floatOopDesc(float value);
    };

    class doubleOopDesc : public primitiveOopDesc<double> {
    public:
        explicit doubleOopDesc(double value);
    };
}
