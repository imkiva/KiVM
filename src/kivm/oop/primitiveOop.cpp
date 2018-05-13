//
// Created by kiva on 2018/2/28.
//

#include <kivm/oop/primitiveOop.h>

namespace kivm {
    intOopDesc::intOopDesc(jint value) : primitiveOopDesc(value) {
    }

    longOopDesc::longOopDesc(jlong value) : primitiveOopDesc(value) {
    }

    floatOopDesc::floatOopDesc(jfloat value) : primitiveOopDesc(value) {
    }

    doubleOopDesc::doubleOopDesc(jdouble value) : primitiveOopDesc(value) {
    }
}
