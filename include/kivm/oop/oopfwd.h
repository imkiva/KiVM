//
// Created by kiva on 2018/2/28.
//
#pragma once

namespace kivm {
    enum oopType {
        INSTANCE_OOP,
        PRIMITIVE_OOP,
        OBJECT_ARRAY_OOP,
        TYPE_ARRAY_OOP,
    };

    enum ValueType {
        BYTE,
        BOOLEAN,
        CHAR,
        SHORT,
        INT,
        FLOAT,
        LONG,
        DOUBLE,
        OBJECT,
        ARRAY,
    };

    class oopDesc;

    class instanceOopDesc;

    class mirrorOopDesc;

    class arrayOopDesc;

    class typeArrayOopDesc;

    class objectArrayOopDesc;

    class intOopDesc;

    class longOopDesc;

    class floatOopDesc;

    class doubleOopDesc;

    using oop = oopDesc *;
    using instanceOop = instanceOopDesc *;
    using mirrorOop = mirrorOopDesc *;
    using arrayOop= arrayOopDesc *;
    using typeArrayOop= typeArrayOopDesc *;
    using objectArrayOop= objectArrayOopDesc *;
    using intOop = intOopDesc *;
    using longOop= longOopDesc *;
    using floatOop = floatOopDesc *;
    using doubleOop = doubleOopDesc *;
}
