//
// Created by kiva on 2018/2/28.
//
#pragma once

#include <kivm/instanceKlass.h>

namespace kivm {
    class MirrorKlass : public InstanceKlass {
    public:
        MirrorKlass() = delete;

        MirrorKlass(const MirrorKlass &) = delete;

        MirrorKlass(MirrorKlass &&) noexcept = delete;

        mirrorOop *new_mirror(Klass *target, mirrorOop loader);
    };
}
