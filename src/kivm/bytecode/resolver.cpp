//
// Created by kiva on 2018/4/6.
//

#include <kivm/bytecode/execution.h>

namespace kivm {
    oop Resolver::resolveJObject(jobject obj) {
        return nullptr;
    }

    instanceOop Resolver::tryResolveInstance(jobject obj) {
        return nullptr;
    }

    arrayOop Resolver::tryResolveArray(jobject obj) {
        return nullptr;
    }

    typeArrayOop Resolver::tryResolveTypeArray(jobject obj) {
        return nullptr;
    }

    objectArrayOop Resolver::tryResolveObjectArray(jobject object) {
        return nullptr;
    }
}
