//
// Created by kiva on 2018/3/22.
//
#include <kivm/runtime/stack.h>
#include <cstring>

namespace kivm {

    SlotArray::SlotArray(int size)
        : _size(size), _elements(nullptr) {
        if (size > 0) {
            this->_elements = new Slot[size];
            memset(this->_elements, '\0', sizeof(Slot) * size);
        }
    }

    SlotArray::~SlotArray() {
        // FIXME: incorrect checksum for freed object - object was probably modified after being freed.
        if (this->_elements != nullptr) {
            delete[] this->_elements;
            this->_elements = nullptr;
        }
    }

    Stack::Stack(int size)
        : _array(size), _sp(0) {
    }

    Locals::Locals(int size)
        : _array(size) {
    }
}
