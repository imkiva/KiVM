//
// Created by kiva on 2018/2/28.
//

#include <kivm/oop/mirrorOop.h>

namespace kivm {

    mirrorOopDesc::mirrorOopDesc(Klass *mirror)
            : instanceOopDesc((InstanceKlass *) BootstrapClassLoader::get()->loadClass(L"java/lang/Class")),
              _mirrorTarget(mirror),
              _mirroringPrimitiveType(ValueType::OBJECT) {
    }
}
