//
// Created by kiva on 2018/2/28.
//
#pragma once

#include <kivm/oop/instanceKlass.h>
#include <kivm/oop/oopfwd.h>

namespace kivm {
    class ArrayKlass : public Klass {
    private:
        ClassLoader *_class_loader;
        mirrorOop _mirror_loader;

        int _dimension;

    public:
        ArrayKlass(ClassLoader *class_loader, int dimension, ClassType class_type);

        ClassLoader *get_class_loader() const {
            return _class_loader;
        }

        int get_dimension() const {
            return _dimension;
        }
    };

    class TypeArrayKlass : public ArrayKlass {
    private:
        ValueType _component_type;

    public:
        TypeArrayKlass(ClassLoader *class_loader, int dimension, ValueType component_type);

        ValueType get_component_type() const {
            return _component_type;
        }
    };

    class ObjectArrayKlass : public ArrayKlass {
    private:
        InstanceKlass * _component_type;

    public:
        ObjectArrayKlass(ClassLoader *class_loader, int dimension, InstanceKlass * component_type);

        InstanceKlass * get_component_type() const {
            return _component_type;
        }
    };
}
