//
// Created by kiva on 2018/3/1.
//

#include "extension_helper.h"

CS_EXTENSION(kivm) // NOLINT
    CS_OBJECT(kivm, kivm_holder) {
        int x;
    };

    CNI_NORMAL(void, hello_world) { // NOLINT
        printf("Hello World: CovScript\n");
    }

    CNI_NORMAL(void, greet, cs::number x) { // NOLINT
        printf("Hello World: %d\n", static_cast<int>(x));
    }

    CNI_NORMAL(void, greet2, cs::number x, cs::number y) { // NOLINT
        printf("Hello World: x = %d, y = %d\n", static_cast<int>(x), static_cast<int>(y));
    }

CS_EXTENSION_END()
