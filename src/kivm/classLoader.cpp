//
// Created by kiva on 2018/2/27.
//

#include <unordered_map>
#include <kivm/classLoader.h>
#include <kivm/system.h>
#include <kivm/oop/klass.h>
#include <shared/lock.h>

namespace kivm {
    static RecursiveLock &get_bootstrap_lock() {
        static RecursiveLock lock;
        return lock;
    }

    Klass *ClassLoader::requireClass(ClassLoader *classLoader, const String &className) {
        if (classLoader == nullptr) {
            // This is a bootstrap class
            classLoader = BootstrapClassLoader::get();
        }

        Klass *loaded_class = classLoader == nullptr
                              ? nullptr
                              : classLoader->loadClass(className);
        if (loaded_class == nullptr) {
            // TODO: throw LinkageError
            PANIC("LinkageError");
        }
        return loaded_class;
    }

    BootstrapClassLoader *BootstrapClassLoader::get() {
        static BootstrapClassLoader classLoader;
        return &classLoader;
    }

    Klass *BootstrapClassLoader::loadClass(const String &class_name) {
        RecursiveLockGuard guard(get_bootstrap_lock());

        // check whether class is already loaded
        auto iter = SystemDictionary::get()->find(class_name);
        if (iter != nullptr) {
            return iter;
        }

        // OK, let's find it!
        auto *klass = BaseClassLoader::loadClass(class_name);
        if (klass != nullptr) {
            SystemDictionary::get()->put(class_name, klass);
            klass->setClassState(ClassState::LOADED);
            klass->linkAndInit();
        }
        return klass;
    }
}
