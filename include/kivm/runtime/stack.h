//
// Created by kiva on 2018/3/21.
//
#pragma once

#include <kivm/runtime/slot.h>
#include <cstdio>

namespace kivm {
    class Stack {
    private:
        Slot *_elements;
        int _size;
        int _sp;

    public:
        explicit Stack(int size);

        ~Stack();
    };
}

