//
// Created by kiva on 2018/3/28.
//
#pragma once

#include <kivm/kivm.h>
#include <kivm/oop/oopfwd.h>
#include <queue>
#include <unordered_map>


namespace kivm {
    namespace java {
        namespace lang {
            class Class {
            private:
                enum ClassMirrorState {
                    FIXED, NOT_FIXED
                };

                static std::unordered_map<String, mirrorOop>& getPrimitiveTypeMirrors();

                static std::queue<String> &getDelayedMirrors();

                static ClassMirrorState &getMirrorState();

            public:
                static void initialize();

                static void mirrorCoreClasses();
            };
        }
    }
}
