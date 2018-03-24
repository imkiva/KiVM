//
// Created by kiva on 2018/3/1.
//

#include <kivm/oop/arrayKlass.h>
#include <sstream>

namespace kivm {

    ArrayKlass::ArrayKlass(ClassLoader *class_loader, mirrorOop java_loader,
                           int dimension, ClassType class_type)
            : _class_loader(class_loader),
              _java_loader(java_loader),
              _dimension(dimension) {
        this->set_type(class_type);
    }

    void ArrayKlass::link_and_init() {
        this->set_state(ClassState::FULLY_INITIALIZED);
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
        ss << value_type_to_primitive_type(component_type);
        this->set_name(ss.str());
    }

    TypeArrayKlass::TypeArrayKlass(ClassLoader *class_loader, TypeArrayKlass *down_type)
            : TypeArrayKlass(class_loader,
                             down_type->get_java_loader(),
                             down_type->get_dimension() + 1,
                             down_type->get_component_type()) {
        this->_down_dimension_type = down_type;
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
        ss << L"L" << component_type->get_name() << L";";
        this->set_name(ss.str());
    }

    ObjectArrayKlass::ObjectArrayKlass(ClassLoader *class_loader, ObjectArrayKlass *down_type)
            : ObjectArrayKlass(class_loader,
                               down_type->get_java_loader(),
                               down_type->get_dimension() + 1,
                               down_type->get_component_type()) {
        this->_down_dimension_type = down_type;
    }
}
