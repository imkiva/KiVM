//
// Created by kiva on 2018/4/28.
//

#include <kivm/kivm.h>
#include <kivm/native/classNames.h>
#include <kivm/native/java_lang_String.h>
#include <kivm/oop/mirrorOop.h>
#include <kivm/oop/primitiveOop.h>
#include <kivm/bytecode/execution.h>
#include <shared/atomic.h>
#include <tuple>
#include <kivm/memory/universe.h>

using namespace kivm;

//union OffsetEncoder {
//    struct {
//        int isStatic;
//        int offset;
//    };
//    jlong result;
//
//    std::tuple<int, bool> decode() {
//        return std::make_tuple(offset, isStatic != 0);
//    };
//
//    OffsetEncoder(int offset, bool isStatic) {
//        this->offset = offset;
//        this->isStatic = isStatic;
//    }
//
//    OffsetEncoder(jlong encoded) {
//        this->result = encoded;
//    }
//};

static jlong encodeOffset(int offset, bool isStatic) {
//    return OffsetEncoder(offset, isStatic).result;
    ++offset;
    if (isStatic) {
        return offset * 2;
    }
    return offset * 2 + 1;
}

static std::tuple<int, bool> decodeOffset(jlong encoded) {
//    return OffsetEncoder(encoded).decode();
    if (encoded % 2 == 0) {
        return std::make_tuple((int) (encoded / 2) - 1, true);
    }
    return std::make_tuple((int) ((encoded - 1) / 2) - 1, false);
};

JAVA_NATIVE void
Java_sun_misc_Unsafe_registerNatives(JNIEnv *env, jclass sun_misc_Unsafe) {
    D("sun/misc/Unsafe.registerNatives()V");
}

JAVA_NATIVE jint
Java_sun_misc_Unsafe_arrayBaseOffset(JNIEnv *env, jobject javaUnsafe, jobject mirror) {
    D("sun/misc/Unsafe.arrayBaseOffset(Ljava/lang/Class;)I");
    return 0;
}

JAVA_NATIVE jint
Java_sun_misc_Unsafe_arrayIndexScale(JNIEnv *env, jobject javaUnsafe,
                                     jobject mirror) {
    D("sun/misc/Unsafe.arrayIndexScale(Ljava/lang/Class;)I");
    return sizeof(intptr_t);
}

JAVA_NATIVE jint
Java_sun_misc_Unsafe_addressSize(JNIEnv *env, jobject javaUnsafe) {
    D("sun/misc/Unsafe.addressSize()I");
    return sizeof(intptr_t);
}

JAVA_NATIVE jlong
Java_sun_misc_Unsafe_objectFieldOffset(JNIEnv *env, jobject javaUnsafe, jobject javaField) {
    auto fieldOop = Resolver::instance(javaField);
    instanceOop p = nullptr;

    if (!fieldOop->getFieldValue(J_FIELD, L"name", L"Ljava/lang/String;", (oop *) &p)) {
        SHOULD_NOT_REACH_HERE();
    }

    String fieldName = java::lang::String::toNativeString(p);
    String fieldDesc;

    if (!fieldOop->getFieldValue(J_FIELD, L"type", L"Ljava/lang/Class;", (oop *) &p)) {
        SHOULD_NOT_REACH_HERE();
    }

    auto fieldTypeMirror = Resolver::mirror(p);
    if (fieldTypeMirror->getMirrorTarget() == nullptr) {
        fieldDesc = valueTypeToPrimitiveTypeDesc(fieldTypeMirror->getMirroringPrimitiveType());

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

    auto fieldOwnerMirror = Resolver::mirror(p);
    if (fieldOwnerMirror->getMirrorTarget() == nullptr
        || fieldOwnerMirror->getMirrorTarget()->getClassType() != ClassType::INSTANCE_CLASS) {
        SHOULD_NOT_REACH_HERE();
    }

    auto owner = (InstanceKlass *) fieldOwnerMirror->getMirrorTarget();
    auto fieldInfo = owner->getThisClassField(fieldName, fieldDesc);
    if (fieldInfo == nullptr) {
        SHOULD_NOT_REACH_HERE();
    }

    auto n = encodeOffset(fieldInfo->_offset, fieldInfo->_field->isStatic());
    D("encode offset: %d %s -> %lld", fieldInfo->_offset,
        fieldInfo->_field->isStatic() ? "true" : "false",
        n);
    return n;
}

JAVA_NATIVE jlong
Java_sun_misc_Unsafe_staticFieldOffset(JNIEnv *env, jobject javaUnsafe, jobject javaField) {
    return Java_sun_misc_Unsafe_objectFieldOffset(env, javaUnsafe, javaField);
}

#define DECODE_OFFSET_AND_OWNER(javaOwner, encodedOffset, fieldType, field) \
    int offset = 0; \
    bool isStatic = false; \
    std::tie(offset, isStatic) = decodeOffset(encodedOffset); \
    D("decode offset: %lld -> %d, %s", encodedOffset, offset, isStatic ? "true" : "false"); \
    auto owner = Resolver::instance(javaOwner); \
    fieldType field = nullptr; \
    if (isStatic) { \
        SHOULD_NOT_REACH_HERE(); \
    } else { \
        if (!owner->getFieldValueUnsafe(offset, (oop *) &result)) { \
            SHOULD_NOT_REACH_HERE(); \
        } \
    }

JAVA_NATIVE jint
Java_sun_misc_Unsafe_getIntVolatile(JNIEnv *env, jobject javaUnsafe, jobject javaOwner, jlong encodedOffset) {
    DECODE_OFFSET_AND_OWNER(javaOwner, encodedOffset, intOop, result);
    return result->getValueVolatile();
}

JAVA_NATIVE jboolean
Java_sun_misc_Unsafe_compareAndSwapInt(JNIEnv *env, jobject javaUnsafe,
                                       jobject javaOwner, jlong encodedOffset,
                                       jint expected, jint update) {
    DECODE_OFFSET_AND_OWNER(javaOwner, encodedOffset, intOop, result);
    auto ptr = result->getValueUnsafe();
    return JBOOLEAN(cmpxchg(ptr, expected, update) == expected);
}

JAVA_NATIVE jlong Java_sun_misc_Unsafe_allocateMemory(JNIEnv *env, jobject javaUnsafe, jlong size) {
    if (size <= 0) {
        return 0;
    }
    return (jlong) Universe::allocCObject(size);
}

JAVA_NATIVE void Java_sun_misc_Unsafe_putLong(JNIEnv *env, jobject javaUnsafe, jlong addr, jlong value) {
    if (addr == 0) {
        return;
    }

    *((jlong *) addr) = value;
}

JAVA_NATIVE jbyte Java_sun_misc_Unsafe_getByte(JNIEnv *env, jobject javaUnsafe, jlong addr) {
    if (addr == 0) {
        return 0;
    }

    return *((jbyte *) addr);
}

