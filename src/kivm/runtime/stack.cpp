//
// Created by kiva on 2018/3/22.
//
#include <kivm/runtime/stack.h>

namespace kivm {

    Stack::Stack(int size)
            : _elements(nullptr), _size(size), _sp(0) {
        this->_elements = new Slot[size];
    }

    Stack::~Stack() {
        delete this->_elements;
    }
}
