//
// Created by kiva on 2018/2/25.
//
#pragma once

#define T_BOOLEAN               4
#define T_CHAR                  5
#define T_FLOAT                 6
#define T_DOUBLE                7
#define T_BYTE                  8
#define T_SHORT                 9
#define T_INT                  10
#define T_LONG                 11

#include <kivm/kivm.h>

namespace kivm {
    enum ClassType {
        INSTANCE_CLASS,
        OBJECT_ARRAY_CLASS,
        TYPE_ARRAY_CLASS,
    };

    enum oopType {
        INSTANCE_OOP,
        PRIMITIVE_OOP,
        OBJECT_ARRAY_OOP,
        TYPE_ARRAY_OOP,
    };

    enum ClassState {
        allocated,
        loaded,
        linked,
        being_initialized,
        fully_initialized,
        initialization_error,
    };

    class Klass {
    private:
        ClassState _state;
        ClassType _type;

    public:
        ClassType type() const { return _type; }

        void set_type(ClassType type) { _type = type; }

        ClassState state() const { return _state; }

        void set_state(ClassState state) { _state = state; }
    };
}

