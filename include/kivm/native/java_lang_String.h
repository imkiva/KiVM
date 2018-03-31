//
// Created by kiva on 2018/3/29.
//
#pragma once

#include <kivm/kivm.h>
#include <kivm/oop/oopfwd.h>
#include <unordered_map>

namespace kivm {
    namespace java {
        namespace lang {
            class InternStringPool {
            private:
                // hash -> string
                std::unordered_map<int, instanceOop> _pool;

            public:
                static InternStringPool *getGlobal();

                instanceOop findOrNew(const kivm::String &string);
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
                static instanceOop from(const kivm::String &string);

                static inline instanceOop intern(const kivm::String &string) {
                    return InternStringPool::getGlobal()->findOrNew(string);
                }
            };
        }
    }
}
