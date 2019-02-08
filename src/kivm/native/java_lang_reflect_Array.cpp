//
// Created by kiva on 2018/5/24.
//

#include <kivm/native/classNames.h>
#include <kivm/bytecode/execution.h>
#include <kivm/oop/arrayOop.h>
#include <kivm/oop/mirrorOop.h>
#include <kivm/kivm.h>

using namespace kivm;

JAVA_NATIVE jarray Java_java_lang_reflect_Array_newArray(JNIEnv *env, jclass unused,
                                                         jclass componentMirror, jint length) {
    auto comp = Resolver::mirror(componentMirror);

    if (comp->isPrimitiveMirror()) {
        auto valueType = comp->getPrimitiveType();
        if (valueType == ValueType::VOID) {
            PANIC("wtf");
        }

        auto arrayClassName = L"[" + valueTypeToPrimitiveTypeDesc(valueType);
        auto klass = (TypeArrayKlass *) BootstrapClassLoader::get()->loadClass(arrayClassName);
        return klass->newInstance(length);
    }

    auto target = comp->getTarget();
    switch (target->getClassType()) {
        case ClassType::INSTANCE_CLASS: {
            auto klass = (InstanceKlass *) target;
            auto arrayClassName = L"[L" + klass->getName() + L";";
            auto arrayKlass = (ObjectArrayKlass *) klass->getClassLoader()->loadClass(arrayClassName);
            return arrayKlass->newInstance(length);
        }

        case ClassType::TYPE_ARRAY_CLASS: {
            auto klass = (TypeArrayKlass *) target;
            auto arrayClassName = L"[" + klass->getName();
            auto arrayKlass = (TypeArrayKlass *) klass->getClassLoader()->loadClass(arrayClassName);
            return arrayKlass->newInstance(length);
        }

        case ClassType::OBJECT_ARRAY_CLASS: {
            auto klass = (ObjectArrayKlass *) target;
            auto arrayClassName = L"[" + klass->getName();
            auto arrayKlass = (ObjectArrayKlass *) klass->getClassLoader()->loadClass(arrayClassName);
            return arrayKlass->newInstance(length);
        }
    }

    SHOULD_NOT_REACH_HERE();
}


