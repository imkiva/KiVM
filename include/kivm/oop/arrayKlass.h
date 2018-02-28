//
// Created by kiva on 2018/2/28.
//
#pragma once

#include <kivm/oop/instanceKlass.h>
#include <kivm/oop/oopfwd.h>

namespace kivm {
    template<typename T, ClassType _TYPE>
    class ArrayKlass : public Klass {
    private:
        ClassLoader *_class_loader;
        int _dimension;
        T _component_type;

    public:
        ArrayKlass(ClassLoader *class_loader, int dimension, T component_type) {
            this->_class_loader = class_loader;
            this->_dimension = dimension;
            this->_component_type = component_type;
            this->set_type(_TYPE);
        }

        ClassLoader *get_class_loader() const {
            return _class_loader;
        }

        int get_dimension() const {
            return _dimension;
        }

        T get_component_type() const {
            return _component_type;
        }
    };

    using TypeArrayKlass = ArrayKlass<ValueType, ClassType::TYPE_ARRAY_CLASS>;
    using ObjectArrayKlass = ArrayKlass<InstanceKlass *, ClassType::OBJECT_ARRAY_CLASS>;
}
