//
// Created by kiva on 2018/3/31.
//

#include <kivm/runtime/constantPool.h>
#include <kivm/oop/instanceKlass.h>
#include <kivm/native/class_names.h>
#include <kivm/native/java_lang_String.h>
#include <kivm/oop/arrayOop.h>
#include <kivm/oop/primitiveOop.h>

namespace kivm {
    RuntimeConstantPool::RuntimeConstantPool(InstanceKlass *instanceKlass)
        : _class_loader(instanceKlass->getClassLoader()) {
    }
}

