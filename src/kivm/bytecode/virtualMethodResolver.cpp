//
// Created by kiva on 2018/4/26.
//

#include <kivm/bytecode/invocationContext.h>

namespace kivm {
    Method *InvocationContext::resolveVirtualMethod(oop thisObject, Method *tagMethod) {
        // if method is not an interface method
        // there's no need to resolve twice
        // just return itself
        if (tagMethod->getMaxLocals() != 0) {
            return tagMethod;
        }

        bool x = tagMethod->getName() == L"charsetForName";

        Method *resolved = nullptr;
        if (thisObject->getClass()->getClassType() == ClassType::INSTANCE_CLASS) {
            auto instanceClass = (InstanceKlass *) thisObject->getClass();
            // FIXME: class B extends abstract class A, class C extends class B
            // FIXME: cannot find abstract method declared in A
            // FIXME: class B implements it and class C uses the implementation provided by B
            resolved = instanceClass->getVirtualMethod(tagMethod->getName(), tagMethod->getDescriptor());
        }
        return resolved;
    }
}
