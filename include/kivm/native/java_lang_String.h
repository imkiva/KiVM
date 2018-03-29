//
// Created by kiva on 2018/3/29.
//
#pragma once

#include <kivm/kivm.h>
#include <kivm/oop/oopfwd.h>

namespace kivm {
    namespace java {
        namespace lang {
            class String {
            public:
                struct Hash {
                    size_t operator()(instanceOop ptr) const noexcept;
                };

                struct EqualTo {
                    bool operator()(instanceOop lhs, instanceOop rhs) const;
                };
            };
        }
    }
}
