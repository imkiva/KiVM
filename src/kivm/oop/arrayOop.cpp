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

    int arrayOopDesc::getDimension() const {
        return ((ArrayKlass *) getClass())->getDimension();
    }

    int arrayOopDesc::getLength() const {
        return static_cast<int>(_elements.size());
    }

    oop arrayOopDesc::getElementAt(int position) const {
        if (position < 0 || position >= getLength()) {
            // TODO: throw ArrayIndexOutOfBoundsException
            PANIC("java.lang.ArrayIndexOutOfBoundsException");
        }
        return _elements[position];
    }

    void arrayOopDesc::setElementAt(int position, oop element) {
        if (position < 0 || position >= getLength()) {
            // TODO: throw ArrayIndexOutOfBoundsException
            PANIC("java.lang.ArrayIndexOutOfBoundsException");
        }
        _elements[position] = element;
    }

    typeArrayOopDesc::typeArrayOopDesc(TypeArrayKlass *array_klass, int length)
        : arrayOopDesc(array_klass, oopType::TYPE_ARRAY_OOP, length) {
    }

    objectArrayOopDesc::objectArrayOopDesc(ObjectArrayKlass *array_klass, int length)
        : arrayOopDesc(array_klass, oopType::OBJECT_ARRAY_OOP, length) {
    }
}
