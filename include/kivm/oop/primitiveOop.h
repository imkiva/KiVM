//
// Created by kiva on 2018/2/28.
//

#include <kivm/oop/oop.h>

namespace kivm {
    template<typename T>
    struct primitive_helper {
        constexpr static ValueType get_value_type() = delete;
    };

    template<>
    struct primitive_helper<int> {
        constexpr static ValueType get_value_type() {
            return ValueType::INT;
        }
    };

    template<>
    struct primitive_helper<long> {
        constexpr static ValueType get_value_type() {
            return ValueType::LONG;
        };
    };

    template<>
    struct primitive_helper<float> {
        constexpr static ValueType get_value_type() {
            return ValueType::FLOAT;
        };
    };

    template<>
    struct primitive_helper<double> {
        constexpr static ValueType get_value_type() {
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

        ValueType get_primitive_type() const {
            return primitive_helper<T>::get_value_type();
        }

        T get_value() const {
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
