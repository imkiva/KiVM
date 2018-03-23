//
// Created by kiva on 2018/2/28.
//

#include <kivm/oop/mirrorKlass.h>
#include <kivm/oop/mirrorOop.h>

namespace kivm {
    mirrorOop mirrorKlass::new_mirror(Klass *target, mirrorOop loader) {
        if (target != nullptr && target->get_name() == L"java/lang/Exception") {
            BootstrapClassLoader::get()->loadClass(L"java/lang/Class");
        }
        auto mirror = new mirrorOopDesc(target);
        if (loader != nullptr) {
//            mirror->set_field_value(L"java/lang/Class:classLoader:Ljava/lang/ClassLoader;", loader);
        }

        return mirror;
    }
}
