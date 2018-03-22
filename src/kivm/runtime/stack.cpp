//
// Created by kiva on 2018/3/22.
//
#include <kivm/runtime/stack.h>

namespace kivm {

    SlotArray::SlotArray(int size)
            : _size(size), _elements(new Slot[size]) {
    }

    SlotArray::~SlotArray() {
        delete[] this->_elements;
    }

    Stack::Stack(int size)
            : SlotArray(size), _sp(0) {
    }

    Locals::Locals(int size)
            : SlotArray(size) {
    }
}
