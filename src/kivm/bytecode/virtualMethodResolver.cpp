//
// Created by kiva on 2018/4/26.
//

#include <kivm/bytecode/invocationContext.h>

namespace kivm {
    Method *InvocationContext::resolveVirtualMethod(oop thisObject, Method *tagMethod) {
        auto thisClass = thisObject->getClass();
        Method *resolved = nullptr;

        if (thisClass->getClassType() == ClassType::INSTANCE_CLASS) {
            auto instanceClass = (InstanceKlass *) thisClass;
            resolved = instanceClass->getVirtualMethod(tagMethod->getName(), tagMethod->getDescriptor());
        } else if (thisClass->getClassType() == ClassType::OBJECT_ARRAY_CLASS
                   || thisClass->getClassType() == ClassType::TYPE_ARRAY_CLASS) {
            resolved = tagMethod;
        } else {
            SHOULD_NOT_REACH_HERE();
        }
        return resolved;
    }
}
