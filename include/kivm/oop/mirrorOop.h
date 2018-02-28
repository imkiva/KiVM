//
// Created by kiva on 2018/2/28.
//
#pragma once

#include <kivm/oop/mirrorKlass.h>
#include <kivm/oop/instanceOop.h>

namespace kivm {
    class mirrorOopDesc : public instanceOopDesc {
    private:
        Klass *_mirror;

    public:
        explicit mirrorOopDesc(Klass *mirror);

        Klass *get_mirror_class() const {
            return _mirror;
        }
    };
}
