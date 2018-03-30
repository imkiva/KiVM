//
// Created by kiva on 2018/3/29.
//
#pragma once

#include <kivm/kivm.h>
#include <kivm/oop/oopfwd.h>

namespace kivm {
    namespace java {
        namespace lang {
            class StringTable {
            public:
                static instanceOop find_or_new(const kivm::String &string);
            };

            class String {
            public:
                struct Hash {
                    int operator()(instanceOop ptr) const noexcept;

                    int operator()(const kivm::String &string) const noexcept;
                };

                struct EqualTo {
                    bool operator()(instanceOop lhs, instanceOop rhs) const;
                };

            public:
                static inline instanceOop intern(const kivm::String &string) {
                    return StringTable::find_or_new(string);
                }
            };
        }
    }
}
