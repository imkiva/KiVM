//
// Created by kiva on 2018/2/28.
//

#include <kivm/oop/arrayOop.h>
#include <kivm/oop/primitiveOop.h>

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
        switch (arrayClass->getComponentType()) {
            case ValueType::INT:
            case ValueType::CHAR:
            case ValueType::BYTE:
            case ValueType::SHORT:
                for (int i = 0; i < getLength(); ++i) {
                    this->_elements[i] = new intOopDesc(0);
                }
                break;
            case ValueType::FLOAT:
                for (int i = 0; i < getLength(); ++i) {
                    this->_elements[i] = new floatOopDesc(0.0f);
                }
                break;
            case ValueType::LONG:
                for (int i = 0; i < getLength(); ++i) {
                    this->_elements[i] = new longOopDesc(0L);
                }
                break;
            case ValueType::DOUBLE:
                for (int i = 0; i < getLength(); ++i) {
                    this->_elements[i] = new doubleOopDesc(0.0);
                }
                break;

            default:
                SHOULD_NOT_REACH_HERE();
        }
    }

    objectArrayOopDesc::objectArrayOopDesc(ObjectArrayKlass *arrayClass, int length)
        : arrayOopDesc(arrayClass, oopType::OBJECT_ARRAY_OOP, length) {
    }
}
