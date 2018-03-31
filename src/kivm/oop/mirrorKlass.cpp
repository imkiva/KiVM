//
// Created by kiva on 2018/2/28.
//

#include <kivm/oop/mirrorKlass.h>
#include <kivm/oop/mirrorOop.h>

namespace kivm {
    mirrorOop mirrorKlass::newMirror(Klass *target, mirrorOop loader) {
        if (target != nullptr && target->getName() == L"java/lang/Exception") {
            BootstrapClassLoader::get()->loadClass(L"java/lang/Class");
        }
        auto mirror = new mirrorOopDesc(target);
        if (loader != nullptr) {
            mirror->setFieldValue(L"java/lang/Class",
                                  L"classLoader",
                                  L"Ljava/lang/ClassLoader;",
                                  loader);
        }

        return mirror;
    }
}
