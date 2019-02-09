//
// Created by kiva on 2018/4/28.
//

#include <kivm/kivm.h>
#include <kivm/native/classNames.h>
#include <kivm/native/java_lang_String.h>
#include <kivm/oop/mirrorOop.h>
#include <kivm/oop/primitiveOop.h>
#include <kivm/oop/arrayOop.h>
#include <kivm/bytecode/execution.h>
#include <shared/atomic.h>
#include <tuple>
#include <kivm/memory/universe.h>

using namespace kivm;

union OffsetEncoder {
    struct {
        int isStatic;
        int offset;
    };
    jlong result{};

    std::tuple<int, bool> decode() {
        return std::make_tuple(offset, isStatic != 0);
    };

    jlong encode() {
        return result;
    }

    OffsetEncoder(int offset, bool isStatic) {
        this->offset = offset;
        this->isStatic = isStatic;
    }

    explicit OffsetEncoder(jlong encoded) {
        this->result = encoded;
    }
};

static jlong encodeOffset(int offset, bool isStatic) {
    return OffsetEncoder(offset, isStatic).encode();
}

static std::tuple<int, bool> decodeOffset(jlong encoded) {
    return OffsetEncoder(encoded).decode();
};

oop *getFieldByOffset(oop owner, int offset, bool isStatic) {
    switch (owner->getMarkOop()->getOopType()) {
        case oopType::OBJECT_ARRAY_OOP:
        case oopType::TYPE_ARRAY_OOP: {
            int arrayIndex = offset / sizeof(intptr_t);
            auto array = Resolver::array(owner);
            return array->getElementUnsafe(arrayIndex);
        }

        case oopType::INSTANCE_OOP: {
            auto instance = Resolver::instance(owner);
            oop *result = nullptr;
            if (isStatic) {
                auto klass = (InstanceKlass *) owner->getClass();
                if (!klass->getStaticFieldValueUnsafe(offset, &result)) {
                    SHOULD_NOT_REACH_HERE();
                }
            } else {
                if (!instance->getFieldValueUnsafe(offset, &result)) {
                    SHOULD_NOT_REACH_HERE();
                }
            }
            return result;
        }

        default:
            SHOULD_NOT_REACH_HERE();
    }
    return nullptr;
}

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
    if (fieldTypeMirror->getTarget() == nullptr) {
        fieldDesc = valueTypeToPrimitiveTypeDesc(fieldTypeMirror->getPrimitiveType());

    } else {
        auto target = fieldTypeMirror->getTarget();
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
    if (fieldOwnerMirror->getTarget() == nullptr
        || fieldOwnerMirror->getTarget()->getClassType() != ClassType::INSTANCE_CLASS) {
        SHOULD_NOT_REACH_HERE();
    }

    auto owner = (InstanceKlass *) fieldOwnerMirror->getTarget();
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

#define DECODE_OFFSET_AND_OWNER(javaOwner, encodedOffset) \
    int offset = 0; \
    bool isStatic = false; \
    std::tie(offset, isStatic) = decodeOffset(encodedOffset); \
    D("decode offset: %lld -> %d, %s", encodedOffset, offset, isStatic ? "true" : "false"); \
    auto owner = Resolver::javaOop(javaOwner)


JAVA_NATIVE jint
Java_sun_misc_Unsafe_getIntVolatile(JNIEnv *env, jobject javaUnsafe, jobject javaOwner, jlong encodedOffset) {
    DECODE_OFFSET_AND_OWNER(javaOwner, encodedOffset);
    oop *addr = getFieldByOffset(owner, offset, isStatic);
    return (*((volatile intOop *) addr))->getValueVolatile();
}

JAVA_NATIVE jobject
Java_sun_misc_Unsafe_getObjectVolatile(JNIEnv *env, jobject javaUnsafe, jobject javaOwner, jlong encodedOffset) {
    DECODE_OFFSET_AND_OWNER(javaOwner, encodedOffset);
    oop *addr = getFieldByOffset(owner, offset, isStatic);
    return *((volatile oop *) addr);
}

JAVA_NATIVE void
Java_sun_misc_Unsafe_putObjectVolatile(JNIEnv *env, jobject javaUnsafe, jobject javaOwner, jlong encodedOffset,
                                       jobject obj) {
    DECODE_OFFSET_AND_OWNER(javaOwner, encodedOffset);
    oop *addr = getFieldByOffset(owner, offset, isStatic);
    *((volatile oop *) addr) = Resolver::javaOop(obj);
}

JAVA_NATIVE jboolean
Java_sun_misc_Unsafe_compareAndSwapInt(JNIEnv *env, jobject javaUnsafe,
                                       jobject javaOwner, jlong encodedOffset,
                                       jint expected, jint update) {
    DECODE_OFFSET_AND_OWNER(javaOwner, encodedOffset);
    oop *addr = getFieldByOffset(owner, offset, isStatic);
    volatile jint *ptr = (*((intOop *) addr))->getValueUnsafe();
    return JBOOLEAN(cmpxchg(ptr, expected, update) == expected);
}

JAVA_NATIVE jboolean
Java_sun_misc_Unsafe_compareAndSwapLong(JNIEnv *env, jobject javaUnsafe,
                                        jobject javaOwner, jlong encodedOffset,
                                        jlong expected, jlong update) {
    DECODE_OFFSET_AND_OWNER(javaOwner, encodedOffset);
    oop *addr = getFieldByOffset(owner, offset, isStatic);
    volatile jlong *ptr = (*((longOop *) addr))->getValueUnsafe();
    return JBOOLEAN(cmpxchg(ptr, expected, update) == expected);
}

JAVA_NATIVE jboolean
Java_sun_misc_Unsafe_compareAndSwapObject(JNIEnv *env, jobject javaUnsafe,
                                          jobject javaOwner, jlong encodedOffset,
                                          jobject expected, jobject update) {
    DECODE_OFFSET_AND_OWNER(javaOwner, encodedOffset);
    auto ptr = (volatile uintptr_t *) getFieldByOffset(owner, offset, isStatic);
    return JBOOLEAN(cmpxchg(ptr, (uintptr_t) expected, (uintptr_t) update) == (uintptr_t) expected);
}

JAVA_NATIVE jlong Java_sun_misc_Unsafe_allocateMemory(JNIEnv *env, jobject javaUnsafe, jlong size) {
    if (size <= 0) {
        return 0;
    }
    auto result = (jlong) Universe::allocCObject((size_t) size);
    D("Unsafe: allocated memory at: %p -> %lld", (void *) result, result);
    return result;
}

JAVA_NATIVE void Java_sun_misc_Unsafe_putLong(JNIEnv *env, jobject javaUnsafe, jlong addr, jlong value) {
    if (addr == 0) {
        return;
    }

    auto ptr = (jlong *) addr;
    D("Unsafe: put long at: %p -> %lld", ptr, addr);
    *ptr = value;
}

JAVA_NATIVE jbyte Java_sun_misc_Unsafe_getByte(JNIEnv *env, jobject javaUnsafe, jlong addr) {
    if (addr == 0) {
        return 0;
    }

    auto ptr = (jbyte *) addr;
    D("Unsafe: get byte at: %p -> %lld", ptr, addr);
    return *ptr;
}

JAVA_NATIVE void Java_sun_misc_Unsafe_freeMemory(JNIEnv *env, jobject javaUnsafe, jlong addr) {
    if (addr == 0) {
        return;
    }

    auto ptr = (void *) addr;
    D("Unsafe: free memory at: %p -> %lld", ptr, addr);
    Universe::deallocCObject(ptr);
}

