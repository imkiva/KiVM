//
// Created by kiva on 2018/3/1.
//

#include <kivm/oop/arrayKlass.h>
#include <kivm/oop/arrayOop.h>
#include <kivm/native/java_lang_Class.h>
#include <sstream>

namespace kivm {

    ArrayKlass::ArrayKlass(ClassLoader *classLoader, mirrorOop javaLoader,
                           int dimension, ClassType classType)
        : _classLoader(classLoader),
          _javaLoader(javaLoader),
          _dimension(dimension) {
        this->setClassType(classType);
        this->setJavaMirror(nullptr);
    }

    void ArrayKlass::linkClass() {
        this->setSuperClass((InstanceKlass *) BootstrapClassLoader::requireClass(
            BootstrapClassLoader::get(), L"java/lang/Object"
        ));
        this->setClassState(ClassState::LINKED);
    }

    void ArrayKlass::initClass() {
        // do nothing
    }

    TypeArrayKlass::TypeArrayKlass(ClassLoader *classLoader, mirrorOop javaLoader,
                                   int dimension, ValueType componentType)
        : ArrayKlass(classLoader, javaLoader, dimension, ClassType::TYPE_ARRAY_CLASS),
          _componentType(componentType),
          _downDimensionType(nullptr) {
        std::wstringstream ss;
        for (int i = 0; i < dimension; ++i) {
            ss << L"[";
        }
        ss << valueTypeToPrimitiveType(componentType);
        this->setName(ss.str());
    }

    TypeArrayKlass::TypeArrayKlass(ClassLoader *classLoader, TypeArrayKlass *downType)
        : TypeArrayKlass(classLoader,
        downType->getJavaMirror(),
        downType->getDimension() + 1,
        downType->getComponentType()) {
        this->_downDimensionType = downType;
    }

    typeArrayOop TypeArrayKlass::newInstance(int length) {
        return new typeArrayOopDesc(this, length);
    }

    void TypeArrayKlass::linkClass() {
        // Type array classes are already
        // mirrored in java::lang::Class::initialize()
        ArrayKlass::linkClass();
    }

    void TypeArrayKlass::copyArrayTo(arrayOop src, arrayOop dest, int srcPos, int destPos, int length) {
        if (src->getClass()->getClassType() != ClassType::TYPE_ARRAY_CLASS
            || dest->getClass()->getClassType() != ClassType::TYPE_ARRAY_CLASS) {
            PANIC("java.lang.ArrayStoreException");
        }

        auto srcOop = (typeArrayOop) src;
        auto destOop = (typeArrayOop) dest;
        auto destClass = (TypeArrayKlass *) destOop->getClass();
        if (destClass->getComponentType() != this->getComponentType()) {
            PANIC("java.lang.ArrayStoreException");
        }

        // Check is all offsets and lengths are non negative
        if (srcPos < 0 || destPos < 0 || length < 0) {
            PANIC("java.lang.ArrayIndexOutOfBoundsException");
        }

        // Check if the ranges are valid
        if ((((unsigned int) length + (unsigned int) srcPos) > (unsigned int) srcOop->getLength())
            || (((unsigned int) length + (unsigned int) destPos) > (unsigned int) destOop->getLength())) {
            PANIC("java.lang.ArrayIndexOutOfBoundsException");
        }

        // Check zero copy
        if (length == 0) {
            return;
        }

        std::copy(srcOop->_elements.begin() + srcPos,
            srcOop->_elements.begin() + srcPos + length,
            destOop->_elements.begin() + destPos);
    }

    ObjectArrayKlass::ObjectArrayKlass(ClassLoader *classLoader, mirrorOop javaLoader,
                                       int dimension, InstanceKlass *componentType)
        : ArrayKlass(classLoader, javaLoader, dimension, ClassType::OBJECT_ARRAY_CLASS),
          _componentType(componentType),
          _downDimensionType(nullptr) {
        std::wstringstream ss;
        for (int i = 0; i < dimension; ++i) {
            ss << L"[";
        }
        ss << L"L" << componentType->getName() << L";";
        this->setName(ss.str());
    }

    ObjectArrayKlass::ObjectArrayKlass(ClassLoader *classLoader, ObjectArrayKlass *downType)
        : ObjectArrayKlass(classLoader,
        downType->getJavaMirror(),
        downType->getDimension() + 1,
        downType->getComponentType()) {
        this->_downDimensionType = downType;
    }

    objectArrayOop ObjectArrayKlass::newInstance(int length) {
        return new objectArrayOopDesc(this, length);
    }

    void ObjectArrayKlass::linkClass() {
        java::lang::Class::createMirror(this, getJavaLoader());
        ArrayKlass::linkClass();
    }

    void ObjectArrayKlass::copyArrayTo(arrayOop src, arrayOop dest, int srcPos, int destPos, int length) {
        PANIC("ObjectArrayKlass::copyArrayTo()");
    }
}
