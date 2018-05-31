//
// Created by kiva on 2018/6/1.
//
#pragma once

#include <kivm/kivm.h>
#include <kivm/oop/primitiveOop.h>

#define INTEGER_CACHE_HIGH (127)
#define INTEGER_CACHE_LOW (-128)
#define INTEGER_CACHE_MAX ((INTEGER_CACHE_HIGH - INTEGER_CACHE_LOW) + 1)

namespace kivm {
    class IntegerCache {
    private:
        static intOop sIntegerPool[INTEGER_CACHE_MAX];

    public:
        static void initialize();

        static intOop valueOf(jint value);
    };
}

