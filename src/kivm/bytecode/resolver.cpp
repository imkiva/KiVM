//
// Created by kiva on 2018/4/6.
//

#include <kivm/bytecode/execution.h>
#include <kivm/oop/instanceOop.h>
#include <kivm/oop/arrayOop.h>
#include <kivm/oop/primitiveOop.h>
#include <kivm/oop/mirrorOop.h>
#include <kivm/runtime/nativeMethodPool.h>
#include <kivm/memory/universe.h>

namespace kivm {
    oop Resolver::javaOop(jobject obj) {
        if (Universe::isHeapObject(obj)) {
            return (oop) obj;
        }
        return nullptr;
    }

    instanceOop Resolver::instance(jobject obj) {
        auto n = javaOop(obj);
        if (n == nullptr) {
            return nullptr;
        }

        if (n->getClass()->getClassType() == ClassType::INSTANCE_CLASS) {
            return (instanceOop) n;
        }
        return nullptr;
    }

    mirrorOop Resolver::mirror(jobject obj) {
        auto n = instance(obj);
        return (mirrorOop) n;
    }

    arrayOop Resolver::array(jarray obj) {
        auto n = javaOop(obj);
        if (n == nullptr) {
            return nullptr;
        }

        if (n->getClass()->getClassType() == ClassType::OBJECT_ARRAY_CLASS
            || n->getClass()->getClassType() == ClassType::TYPE_ARRAY_CLASS) {
            return (arrayOop) n;
        }
        return nullptr;
    }

    typeArrayOop Resolver::typeArray(jarray obj) {
        auto n = javaOop(obj);
        if (n == nullptr) {
            return nullptr;
        }

        if (n->getClass()->getClassType() == ClassType::TYPE_ARRAY_CLASS) {
            return (typeArrayOop) n;
        }
        return nullptr;
    }

    objectArrayOop Resolver::objectArray(jobjectArray obj) {
        auto n = javaOop(obj);
        if (n == nullptr) {
            return nullptr;
        }

        if (n->getClass()->getClassType() == ClassType::OBJECT_ARRAY_CLASS) {
            return (objectArrayOop) n;
        }
        return nullptr;
    }

    void *Resolver::nativePointer(Method *method) {
        return NativeMethodPool::get()->resolve(method);
    }

    Klass *Resolver::javaClass(jclass clazz) {
        if (clazz != nullptr) {
            return (Klass *) clazz;
        }
        return nullptr;
    }

    InstanceKlass *Resolver::instanceClass(jclass clazz) {
        auto klass = Resolver::javaClass(clazz);
        if (klass == nullptr) {
            return nullptr;
        }

        if (klass->getClassType() == ClassType::INSTANCE_CLASS) {
            return (InstanceKlass *) klass;
        }
        return nullptr;
    }
}
