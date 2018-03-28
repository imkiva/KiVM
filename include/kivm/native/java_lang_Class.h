//
// Created by kiva on 2018/3/28.
//
#pragma once

#include <kivm/kivm.h>
#include <queue>
#include <kivm/oop/oopfwd.h>

namespace kivm {
    namespace java {
        namespace lang {
            class Class {
            private:
                enum ClassMirrorState {
                    FIXED, NOT_FIXED
                };

                static std::queue<String> &delayed_mirrors();

                static ClassMirrorState &mirror_state();

            public:
                static void initialize();

                static void mirror_core_classes();

                static mirrorOop mirror_for_primitive_type();
            };
        }
    }
}
