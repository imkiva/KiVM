//
// Created by kiva on 2018/2/28.
//
#pragma once

#include <kivm/oop/instanceKlass.h>
#include <kivm/oop/oopfwd.h>

namespace kivm {
    class ArrayKlass : public Klass {
    private:
        ClassLoader *_classLoader;
        mirrorOop _javaLoader;

        int _dimension;

    public:
        ArrayKlass(ClassLoader *classLoader, mirrorOop javaLoader,
                   int dimension, ClassType classType);

        ClassLoader *getClassLoader() const {
            return _classLoader;
        }

        int getDimension() const {
            return _dimension;
        }

        mirrorOop getJavaLoader() {
            return _javaLoader;
        }

        bool isObjectArray() const {
            return getClassType() == ClassType::OBJECT_ARRAY_CLASS;
        }

        void linkAndInit() override;
    };

    class TypeArrayKlass : public ArrayKlass {
    private:
        ValueType _componentType;

        // Only available when dimension > 1
        TypeArrayKlass *_downDimensionType;

    public:
        TypeArrayKlass(ClassLoader *classLoader, mirrorOop javaLoader,
                       int dimension, ValueType componentType);

        TypeArrayKlass(ClassLoader *classLoader, TypeArrayKlass *downType);

        ValueType getComponentType() const {
            return _componentType;
        }

        void linkAndInit() override;

        TypeArrayKlass *getDownDimensionType() const {
            return _downDimensionType;
        }

        typeArrayOop newInstance(int length);
    };

    class ObjectArrayKlass : public ArrayKlass {
    private:
        InstanceKlass *_componentType;

        // Only available when dimension > 1
        ObjectArrayKlass *_downDimensionType;

    public:
        ObjectArrayKlass(ClassLoader *classLoader, mirrorOop javaLoader,
                         int dimension, InstanceKlass *componentType);

        ObjectArrayKlass(ClassLoader *classLoader, ObjectArrayKlass *downType);

        InstanceKlass *getComponentType() const {
            return _componentType;
        }

        void linkAndInit() override;

        ObjectArrayKlass *getDownDimensionType() const {
            return _downDimensionType;
        }

        objectArrayOop newInstance(int length);
    };
}
