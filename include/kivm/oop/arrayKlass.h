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
        mirrorOop _java_loader;

        int _dimension;

    public:
        ArrayKlass(ClassLoader *class_loader, mirrorOop java_loader,
                   int dimension, ClassType class_type);

        ClassLoader *get_class_loader() const {
            return _class_loader;
        }

        int get_dimension() const {
            return _dimension;
        }

        mirrorOop get_java_loader() {
            return _java_loader;
        }

        void link_and_init() override;

        virtual bool is_object_array() = 0;
    };

    class TypeArrayKlass : public ArrayKlass {
    private:
        ValueType _component_type;

        // Only available when dimension > 1
        TypeArrayKlass *_down_dimension_type;

    public:
        TypeArrayKlass(ClassLoader *class_loader, mirrorOop java_loader,
                       int dimension, ValueType component_type);

        TypeArrayKlass(ClassLoader *class_loader, TypeArrayKlass *down_type);

        ValueType get_component_type() const {
            return _component_type;
        }

        bool is_object_array() override {
            return false;
        }
    };

    class ObjectArrayKlass : public ArrayKlass {
    private:
        InstanceKlass *_component_type;

        // Only available when dimension > 1
        ObjectArrayKlass *_down_dimension_type;

    public:
        ObjectArrayKlass(ClassLoader *class_loader, mirrorOop java_loader,
                         int dimension, InstanceKlass *component_type);

        ObjectArrayKlass(ClassLoader *class_loader, ObjectArrayKlass *down_type);

        InstanceKlass *get_component_type() const {
            return _component_type;
        }

        bool is_object_array() override {
            return true;
        }
    };
}
