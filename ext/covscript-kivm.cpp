//
// Created by kiva on 2018/3/1.
//

#include "extension_helper.h"

CS_EXTENSION(kivm)
    struct kivm_holder {
    };

    CNI_NORMAL(void, hello_world) {
        printf("Hello World: CovScript\n");
    }

    CNI_NORMAL(void, greet, cs::number x) {
        printf("Hello World: %d\n", static_cast<int>(x));
    }

    CNI_NORMAL(void, greet2, cs::number x, cs::number y) {
        printf("Hello World: x = %d, y = %d\n", static_cast<int>(x), static_cast<int>(y));
    }

    CS_ENABLE_THIS(kivm, kivm_holder)

CS_EXTENSION_END()