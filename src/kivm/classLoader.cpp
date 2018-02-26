//
// Created by kiva on 2018/2/27.
//

#include <kivm/classLoader.h>
#include <kivm/klass.h>

namespace kivm {

    BootstrapClassLoader *BootstrapClassLoader::get_class_loader() {
        return nullptr;
    }

    Klass *BootstrapClassLoader::loadClass(const String &class_name) {
        return nullptr;
    }
}
