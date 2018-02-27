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

    class intOopDesc;

    class longOopDesc;

    class floatOopDesc;

    class doubleOopDesc;

    typedef oopDesc *oop;
    typedef instanceOopDesc *instanceOop;
    typedef mirrorOopDesc *mirrorOop;
    typedef arrayOopDesc *arrayOop;
    typedef intOopDesc *intOop;
    typedef longOopDesc *longOop;
    typedef floatOopDesc *floatOop;
    typedef doubleOopDesc *doubleOop;
}
