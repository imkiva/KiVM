//
// Created by kiva on 2018/3/1.
//

#include <kivm/oop/arrayKlass.h>
#include <kivm/oop/arrayOop.h>
#include <sstream>
#include <kivm/native/java_lang_Class.h>

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

    void TypeArrayKlass::copyArrayTo(arrayOop desc, int secPos, int destPos, int length) {
        PANIC("TypeArrayKlass::copyArrayTo()");
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

    void ObjectArrayKlass::copyArrayTo(arrayOop desc, int secPos, int destPos, int length) {
        PANIC("ObjectArrayKlass::copyArrayTo()");
    }
}
