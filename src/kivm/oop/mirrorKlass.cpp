//
// Created by kiva on 2018/2/28.
//

#include <kivm/oop/mirrorKlass.h>
#include <kivm/oop/mirrorOop.h>

namespace kivm {
    mirrorOop mirrorKlass::newMirror(Klass *target, mirrorOop loader) {
        if (target != nullptr && target->getName() == "java/lang/Exception") {
            BootstrapClassLoader::get()->loadClass("java/lang/Class");
        }
        auto mirror = new mirrorOopDesc(target);
        if (loader != nullptr) {
            mirror->setFieldValue("java/lang/Class",
                                  "classLoader",
                                  "Ljava/lang/ClassLoader;",
                                  loader);
        }

        return mirror;
    }
}
