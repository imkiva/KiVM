//
// Created by kiva on 2018/3/29.
//
#pragma once

#include <kivm/kivm.h>
#include <kivm/oop/oopfwd.h>
#include <sparsepp/spp.h>

namespace kivm {
    class CopyingHeap;

    namespace java {
        namespace lang {
            struct StringHash {
                int operator()(instanceOop ptr) const noexcept;

                int operator()(const kivm::String &string) const noexcept;
            };

            struct StringEqualTo {
                bool operator()(instanceOop lhs, instanceOop rhs) const;
            };

            class InternStringPool {
                friend class kivm::CopyingHeap;

            private:
                // hash -> string
                spp::sparse_hash_map<int, instanceOop> _pool;

            public:
                static InternStringPool *getGlobal();

                instanceOop findOrNew(const kivm::String &string);
            };

            class String {
            public:
                static instanceOop from(const kivm::String &string);

                static kivm::String toNativeString(instanceOop stringOop);

                static inline instanceOop intern(const kivm::String &string) {
                    return InternStringPool::getGlobal()->findOrNew(string);
                }
            };
        }
    }
}
