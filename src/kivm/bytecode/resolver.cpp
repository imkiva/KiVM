//
// Created by kiva on 2018/4/6.
//

#include <kivm/bytecode/execution.h>
#include <kivm/oop/instanceOop.h>
#include <kivm/oop/arrayOop.h>
#include <kivm/oop/primitiveOop.h>
#include <kivm/oop/mirrorOop.h>

namespace kivm {
    oop Resolver::resolveJObject(jobject obj) {
        if (obj == nullptr) {
            return nullptr;
        }

        // TODO: check whether obj is an oop
        return (oop) obj;
    }

    instanceOop Resolver::tryResolveInstance(jobject obj) {
        auto n = resolveJObject(obj);
        if (n == nullptr) {
            return nullptr;
        }

        if (n->getClass()->getClassType() == ClassType::INSTANCE_CLASS) {
            return (instanceOop) n;
        }
        return nullptr;
    }

    typeArrayOop Resolver::tryResolveTypeArray(jobject obj) {
        auto n = resolveJObject(obj);
        if (n == nullptr) {
            return nullptr;
        }

        if (n->getClass()->getClassType() == ClassType::TYPE_ARRAY_CLASS) {
            return (typeArrayOop) n;
        }
        return nullptr;
    }

    objectArrayOop Resolver::tryResolveObjectArray(jobject obj) {
        auto n = resolveJObject(obj);
        if (n == nullptr) {
            return nullptr;
        }

        if (n->getClass()->getClassType() == ClassType::OBJECT_ARRAY_CLASS) {
            return (objectArrayOop) n;
        }
        return nullptr;
    }
}
