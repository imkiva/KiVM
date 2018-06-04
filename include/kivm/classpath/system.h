//
// Created by kiva on 2018/3/28.
//
#pragma once

#include <kivm/kivm.h>
#include <shared/lock.h>
#include <unordered_map>

namespace kivm {
    class Klass;

    class SystemDictionary {
    private:
        std::unordered_map<String, Klass *> _classes;
        Lock _lock;

    public:
        static SystemDictionary *get();

        Klass *find(const String &name);

        void put(const String &name, Klass *klass);

        inline const std::unordered_map<String, Klass *> &getLoadedClasses() const {
            return _classes;
        };
    };
}
