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
            enum ClassMirrorState {
                FIXED, NOT_FIXED
            };

            class Class {
            private:
                static std::queue<String>& get_delayed_mirrors();

                static ClassMirrorState& get_mirror_state();

            public:
                static void initialize();

                static void fix_mirrors();

                static mirrorOop mirror_for_primitive_type();
            };
        }
    }
}
