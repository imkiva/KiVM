//
// Created by kiva on 2018/2/28.
//
#pragma once

#include <kivm/oop/instanceKlass.h>

namespace kivm {
    class mirrorKlass : public InstanceKlass {
    public:
        mirrorKlass() = delete;

        mirrorKlass(const mirrorKlass &) = delete;

        mirrorKlass(mirrorKlass &&) noexcept = delete;

        static mirrorOop newMirror(Klass *target, mirrorOop loader);
    };
}
