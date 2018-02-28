//
// Created by kiva on 2018/2/28.
//

#include <kivm/oop/arrayOop.h>

namespace kivm {
    arrayOopDesc::arrayOopDesc(ArrayKlass *array_klass, oopType type, int length)
            : oopDesc(array_klass, type) {
        _elements.resize(static_cast<unsigned>(length));
        _elements.shrink_to_fit();
    }

    int arrayOopDesc::get_dimension() const {
        return ((ArrayKlass *) get_klass())->get_dimension();
    }

    int arrayOopDesc::get_length() const {
        return static_cast<int>(_elements.size());
    }

    oop arrayOopDesc::get_element_at(int position) const {
        if (position < 0 || position >= get_length()) {
            // TODO: throw ArrayIndexOutOfBoundsException
            return nullptr;
        }
        return _elements[position];
    }

    typeArrayOopDesc::typeArrayOopDesc(ArrayKlass *array_klass, int length)
            : arrayOopDesc(array_klass, oopType::TYPE_ARRAY_OOP, length) {
    }

    objectArrayOopDesc::objectArrayOopDesc(ArrayKlass *array_klass, int length)
            : arrayOopDesc(array_klass, oopType::OBJECT_ARRAY_OOP, length) {
    }
}
