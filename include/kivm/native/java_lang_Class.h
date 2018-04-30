//
// Created by kiva on 2018/3/28.
//
#pragma once

#include <kivm/kivm.h>
#include <kivm/oop/oopfwd.h>
#include <queue>
#include <unordered_map>
#include <kivm/oop/klass.h>

namespace kivm {
    namespace java {
        namespace lang {
            class Class {
            private:
                enum ClassMirrorState {
                    FIXED, NOT_FIXED
                };

                static std::unordered_map<kivm::String, mirrorOop> &getPrimitiveTypeMirrors();

                static std::queue<kivm::String> &getDelayedMirrors();

                static std::queue<kivm::Klass *> &getDelayedArrayClassMirrors();

                static ClassMirrorState &getMirrorState();

            public:
                static void initialize();

                static void mirrorCoreAndDelayedClasses();

                static void mirrorDelayedArrayClasses();

                static void createMirror(Klass *klass, mirrorOop javaLoader);

                static mirrorOop findPrimitiveTypeMirror(const kivm::String &signature);
            };
        }
    }
}
