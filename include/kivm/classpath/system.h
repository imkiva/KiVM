//
// Created by kiva on 2018/3/28.
//
#pragma once

#include <kivm/kivm.h>
#include <shared/lock.h>
#include <sparsepp/spp.h>

namespace kivm {
    class Klass;

    class SystemDictionary {
    private:
        spp::sparse_hash_map<String, Klass *> _classes;
        Lock _lock;

    public:
        static SystemDictionary *get();

        Klass *find(const String &name);

        void put(const String &name, Klass *klass);

        inline const spp::sparse_hash_map<String, Klass *> &getLoadedClasses() const {
            return _classes;
        };
    };
}
