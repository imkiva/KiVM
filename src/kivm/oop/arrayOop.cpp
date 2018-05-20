//
// Created by kiva on 2018/2/28.
//

#include <kivm/oop/arrayOop.h>

namespace kivm {
    arrayOopDesc::arrayOopDesc(ArrayKlass *arrayClass, oopType type, int length)
        : oopDesc(arrayClass, type) {
        _elements.resize(static_cast<unsigned>(length));
    }

    oop arrayOopDesc::getElementAt(int position) const {
        if (position < 0 || position >= getLength()) {
            arrayIndexOutOfBounds(position);
            return nullptr;
        }
        return _elements[position];
    }

    void arrayOopDesc::setElementAt(int position, oop element) {
        if (position < 0 || position >= getLength()) {
            arrayIndexOutOfBounds(position);
            return;
        }
        _elements[position] = element;
    }

    void arrayOopDesc::arrayIndexOutOfBounds(int position) const {
        // TODO: throw ArrayIndexOutOfBoundsException
        PANIC("java.lang.ArrayIndexOutOfBoundsException: length is %d, but index is %d",
              getLength(), position);
    }

    typeArrayOopDesc::typeArrayOopDesc(TypeArrayKlass *arrayClass, int length)
        : arrayOopDesc(arrayClass, oopType::TYPE_ARRAY_OOP, length) {
    }

    objectArrayOopDesc::objectArrayOopDesc(ObjectArrayKlass *arrayClass, int length)
        : arrayOopDesc(arrayClass, oopType::OBJECT_ARRAY_OOP, length) {
    }
}
