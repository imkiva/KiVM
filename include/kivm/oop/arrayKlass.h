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

        ClassLoader *getClassLoader() const {
            return _class_loader;
        }

        int getDimension() const {
            return _dimension;
        }

        mirrorOop getJavaLoader() {
            return _java_loader;
        }

        void linkAndInit() override;

        virtual bool isObjectArray() = 0;
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

        ValueType getComponentType() const {
            return _component_type;
        }

        bool isObjectArray() override {
            return false;
        }

        TypeArrayKlass *getDownDimensionType() const {
            return _down_dimension_type;
        }

        typeArrayOop newInstance(int length);
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

        InstanceKlass *getComponentType() const {
            return _component_type;
        }

        bool isObjectArray() override {
            return true;
        }

        ObjectArrayKlass *getDownDimensionType() const {
            return _down_dimension_type;
        }

        objectArrayOop newInstance(int length);
    };
}
