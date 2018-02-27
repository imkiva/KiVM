//
// Created by kiva on 2018/2/28.
//

#include <kivm/oop/instanceOop.h>

namespace kivm {

    instanceOopDesc::instanceOopDesc(InstanceKlass *klass)
            : oopDesc(klass, oopType::INSTANCE_OOP) {
    }
}
