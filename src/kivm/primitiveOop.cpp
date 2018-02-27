//
// Created by kiva on 2018/2/28.
//

#include <kivm/primitiveOop.h>

namespace kivm {
    intOopDesc::intOopDesc(int value) : primitiveOopDesc(value) {
    }

    longOopDesc::longOopDesc(long value) : primitiveOopDesc(value) {
    }

    floatOopDesc::floatOopDesc(float value) : primitiveOopDesc(value) {
    }

    doubleOopDesc::doubleOopDesc(double value) : primitiveOopDesc(value) {
    }
}
