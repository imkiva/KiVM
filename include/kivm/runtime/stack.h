//
// Created by kiva on 2018/3/21.
//
#pragma once

#include <kivm/runtime/slot.h>

namespace kivm {
    class SlotArray {
    protected:
        Slot *_elements;
        int _size;

        explicit SlotArray(int size);

    public:
        virtual ~SlotArray();
    };

    class Stack : public SlotArray {
    private:
        int _sp;

    public:
        explicit Stack(int size);

        ~Stack() override = default;
    };

    class Locals : public SlotArray {
    public:
        explicit Locals(int size);

        ~Locals() override = default;
    };
}

