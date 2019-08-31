//
// Created by kiva on 2018/2/27.
//

#include <sparsepp/spp.h>
#include <kivm/classpath/classLoader.h>
#include <kivm/classpath/system.h>
#include <kivm/oop/klass.h>
#include <shared/lock.h>

namespace kivm {
    static RecursiveLock &bootstrapLock() {
        static RecursiveLock lock;
        return lock;
    }

    Klass *ClassLoader::requireClass(ClassLoader *classLoader, const String &className) {
        if (classLoader == nullptr) {
            // This is a bootstrap class
            classLoader = BootstrapClassLoader::get();
        }

        Klass *loadedClass = classLoader == nullptr
                             ? nullptr
                             : classLoader->loadClass(className);
        if (loadedClass == nullptr) {
            // TODO: throw LinkageError
            PANIC("LinkageError");
        }
        return loadedClass;
    }

    ClassLoader *ClassLoader::getCurrentClassLoader() {
        // TODO: support user-defined class loader
        return BootstrapClassLoader::get();
    }

    BootstrapClassLoader *BootstrapClassLoader::get() {
        static BootstrapClassLoader classLoader;
        return &classLoader;
    }

    Klass *BootstrapClassLoader::loadClass(const String &className) {
        RecursiveLockGuard guard(bootstrapLock());

        // check whether class is already loaded
        auto iter = SystemDictionary::get()->find(className);
        if (iter != nullptr) {
            return iter;
        }

        // OK, let's find it!
        auto *klass = BaseClassLoader::loadClass(className);
        if (klass != nullptr) {
            SystemDictionary::get()->put(className, klass);
            klass->setClassState(ClassState::LOADED);
            klass->linkClass();
        }
        return klass;
    }

    Klass *BootstrapClassLoader::loadClass(u1 *classBytes, size_t classSize) {
        RecursiveLockGuard guard(bootstrapLock());
        Klass *klass = BaseClassLoader::loadClass(classBytes, classSize);
        if (klass == nullptr) {
            return nullptr;
        }

        auto iter = SystemDictionary::get()->find(klass->getName());
        if (iter != nullptr) {
            delete klass;
            return iter;
        }

        SystemDictionary::get()->put(klass->getName(), klass);
        klass->setClassState(ClassState::LOADED);
        klass->linkClass();
        return klass;
    }
}
