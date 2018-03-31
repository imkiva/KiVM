//
// Created by kiva on 2018/3/1.
//

#include <kivm/oop/arrayKlass.h>
#include <kivm/oop/arrayOop.h>
#include <sstream>

namespace kivm {

    ArrayKlass::ArrayKlass(ClassLoader *class_loader, mirrorOop java_loader,
                           int dimension, ClassType class_type)
        : _class_loader(class_loader),
          _java_loader(java_loader),
          _dimension(dimension) {
        this->setClassType(class_type);
    }

    void ArrayKlass::linkAndInit() {
        this->setClassState(ClassState::LINKED);
    }

    TypeArrayKlass::TypeArrayKlass(ClassLoader *class_loader, mirrorOop java_loader,
                                   int dimension, ValueType component_type)
        : ArrayKlass(class_loader, java_loader, dimension, ClassType::TYPE_ARRAY_CLASS),
          _component_type(component_type),
          _down_dimension_type(nullptr) {
        std::wstringstream ss;
        for (int i = 0; i < dimension; ++i) {
            ss << L"[";
        }
        ss << valueTypeToPrimitiveType(component_type);
        this->setName(ss.str());
    }

    TypeArrayKlass::TypeArrayKlass(ClassLoader *class_loader, TypeArrayKlass *down_type)
        : TypeArrayKlass(class_loader,
                         down_type->getJavaMirror(),
                         down_type->getDimension() + 1,
                         down_type->getComponentType()) {
        this->_down_dimension_type = down_type;
    }

    typeArrayOop TypeArrayKlass::newInstance(int length) {
        return new typeArrayOopDesc(this, length);
    }

    ObjectArrayKlass::ObjectArrayKlass(ClassLoader *class_loader, mirrorOop java_loader,
                                       int dimension, InstanceKlass *component_type)
        : ArrayKlass(class_loader, java_loader, dimension, ClassType::OBJECT_ARRAY_CLASS),
          _component_type(component_type),
          _down_dimension_type(nullptr) {
        std::wstringstream ss;
        for (int i = 0; i < dimension; ++i) {
            ss << L"[";
        }
        ss << L"L" << component_type->getName() << L";";
        this->setName(ss.str());
    }

    ObjectArrayKlass::ObjectArrayKlass(ClassLoader *class_loader, ObjectArrayKlass *down_type)
        : ObjectArrayKlass(class_loader,
                           down_type->getJavaMirror(),
                           down_type->getDimension() + 1,
                           down_type->getComponentType()) {
        this->_down_dimension_type = down_type;
    }
}
