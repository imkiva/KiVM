//
// Created by kiva on 2018/4/28.
//

#include <kivm/kivm.h>
#include <kivm/native/classNames.h>
#include <kivm/native/java_lang_String.h>
#include <kivm/oop/mirrorOop.h>
#include <kivm/bytecode/execution.h>

using namespace kivm;

JAVA_NATIVE void Java_sun_misc_Unsafe_registerNatives(JNIEnv *env, jclass sun_misc_Unsafe) {
    D("sun/misc/Unsafe.registerNatives()V");
}

JAVA_NATIVE jint Java_sun_misc_Unsafe_arrayBaseOffset(JNIEnv *env, jobject javaUnsafe,
                                                      jobject mirror) {
    D("sun/misc/Unsafe.arrayBaseOffset(Ljava/lang/Class;)I");
    return 0;
}

JAVA_NATIVE jint Java_sun_misc_Unsafe_arrayIndexScale(JNIEnv *env, jobject javaUnsafe,
                                                      jobject mirror) {
    D("sun/misc/Unsafe.arrayIndexScale(Ljava/lang/Class;)I");
    return sizeof(intptr_t);
}

JAVA_NATIVE jint Java_sun_misc_Unsafe_addressSize(JNIEnv *env, jobject javaUnsafe) {
    D("sun/misc/Unsafe.addressSize()I");
    return sizeof(intptr_t);
}

JAVA_NATIVE jlong Java_sun_misc_Unsafe_objectFieldOffset(JNIEnv *env, jobject javaUnsafe, jobject javaField) {
    auto fieldOop = Resolver::resolveInstance(javaField);
    instanceOop p = nullptr;

    if (!fieldOop->getFieldValue(J_FIELD, L"name", L"Ljava/lang/String;", (oop *) &p)) {
        SHOULD_NOT_REACH_HERE();
    }

    String fieldName = java::lang::String::toNativeString(p);
    String fieldDesc;

    if (!fieldOop->getFieldValue(J_FIELD, L"type", L"Ljava/lang/Class;", (oop *) &p)) {
        SHOULD_NOT_REACH_HERE();
    }

    auto fieldTypeMirror = Resolver::resolveMirror(p);
    if (fieldTypeMirror->getMirrorTarget() == nullptr) {
        fieldDesc = valueTypeToPrimitiveTypeName(fieldTypeMirror->getMirroringPrimitiveType());

    } else {
        auto target = fieldTypeMirror->getMirrorTarget();
        switch (target->getClassType()) {
            case ClassType::INSTANCE_CLASS:
                fieldDesc = L"L" + target->getName() + L";";
                break;
            case ClassType::TYPE_ARRAY_CLASS:
            case ClassType::OBJECT_ARRAY_CLASS:
                fieldDesc = target->getName();
                break;
        }
    }

    if (!fieldOop->getFieldValue(J_FIELD, L"clazz", L"Ljava/lang/Class;", (oop *) &p)) {
        SHOULD_NOT_REACH_HERE();
    }

    auto fieldOwnerMirror = Resolver::resolveMirror(p);
    if (fieldOwnerMirror->getMirrorTarget() == nullptr
        || fieldOwnerMirror->getMirrorTarget()->getClassType() != ClassType::INSTANCE_CLASS) {
        SHOULD_NOT_REACH_HERE();
    }

    auto owner = (InstanceKlass *) fieldOwnerMirror->getMirrorTarget();
    auto fieldInfo = owner->getThisClassField(fieldName, fieldDesc);
    if (fieldInfo == nullptr) {
        SHOULD_NOT_REACH_HERE();
    }
    return fieldInfo->_offset;
}

