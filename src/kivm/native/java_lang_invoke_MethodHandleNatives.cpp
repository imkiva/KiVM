//
// Created by kiva on 2019-02-04.
//

#include <kivm/kivm.h>
#include <kivm/native/classNames.h>
#include <kivm/bytecode/execution.h>
#include <kivm/oop/primitiveOop.h>
#include <kivm/oop/mirrorOop.h>
#include <kivm/oop/arrayOop.h>
#include <list>
#include <algorithm>
#include <sstream>
#include <set>

using namespace kivm;

// copied from MemberName.java
constexpr int KIND_METHOD = 65536;
constexpr int KIND_CTOR = 131072;
constexpr int KIND_FIELD = 262144;

constexpr bool isMethod(int mask) {
    return (mask & KIND_METHOD) == KIND_METHOD;
}

constexpr bool isConstructor(int mask) {
    return (mask & KIND_CTOR) == KIND_CTOR;
}

constexpr bool isField(int mask) {
    return (mask & KIND_FIELD) == KIND_FIELD;
}

static String MEMBER_NAME = L"java/lang/invoke/MemberName";
Lock MEMBER_NAME_CACHE_LOCK;
std::set<instanceOop> MEMBER_NAME_CACHE;

static bool isSpecializedMethod(const String &methodName) {
    static std::list<String> NAME_LIST{L"invoke", L"invokeBasic", L"invokeExact",
                                       L"invokeWithArguments", L"linkToSpecial",
                                       L"linkToStatic", L"linkToVirtual",
                                       L"linkToInterface"};
    return std::find(NAME_LIST.begin(), NAME_LIST.end(), methodName) != NAME_LIST.end();
}

static String toClassName(mirrorOop mirror) {
    if (mirror->isPrimitiveMirror()) {
        return valueTypeToPrimitiveTypeDesc(mirror->getPrimitiveType());

    } else {
        auto target = mirror->getTarget();
        switch (target->getClassType()) {
            case ClassType::INSTANCE_CLASS:
                return L"L" + target->getName() + L";";
            case ClassType::OBJECT_ARRAY_CLASS:
            case ClassType::TYPE_ARRAY_CLASS:
                return target->getName();
            default:
                SHOULD_NOT_REACH_HERE();
        }
    }
}

static String resolveDescriptor(InstanceKlass *klass, const String &name, instanceOop typeObj) {
    static auto MethodHandle = (InstanceKlass *) BootstrapClassLoader::get()->loadClass(J_METHODHANDLE);
    static auto MethodType = (InstanceKlass *) BootstrapClassLoader::get()->loadClass(J_METHODTYPE);

    if (klass == MethodHandle) {
        if (isSpecializedMethod(name)) {
            return L"([Ljava/lang/Object;)Ljava/lang/Object;";
        }
    }

    auto typeClass = typeObj->getClass();

    if (typeClass == Global::_Class) {
        auto mirror = Resolver::mirror(typeObj);
        return toClassName(mirror);
    }

    if (typeClass == MethodType) {
        std::wstringstream ss;
        oop object = nullptr;
        ss << L"(";

        if (!typeObj->getFieldValue(J_METHODTYPE, L"ptypes", L"[Ljava/lang/Class;", &object)) {
            SHOULD_NOT_REACH_HERE();
        }

        auto params = Resolver::objectArray(object);
        if (!params) {
            SHOULD_NOT_REACH_HERE();
        }

        for (int i = 0; i < params->getLength(); ++i) {
            ss << toClassName(Resolver::mirror(params->getElementAt(i)));
        }

        ss << L")";

        if (!typeObj->getFieldValue(J_METHODTYPE, L"rtype", L"Ljava/lang/Class;", &object)) {
            SHOULD_NOT_REACH_HERE();
        }

        auto retType = Resolver::mirror(object);
        if (!retType) {
            SHOULD_NOT_REACH_HERE();
        }

        ss << toClassName(retType);
        return ss.str();
    }

    if (typeClass == Global::_String) {
        SHOULD_NOT_REACH_HERE_M("Work in progress");
    }

    SHOULD_NOT_REACH_HERE();
}

static Method *resolveTargetMethod(InstanceKlass *klass, int refKind,
                                   const String &methodName,
                                   const String &descriptor) {
    switch (refKind) {
        case REF_invokeVirtual:
            return klass->getVirtualMethod(methodName, descriptor);
        case REF_invokeStatic:
            return klass->getThisClassMethod(methodName, descriptor);
        default:
            SHOULD_NOT_REACH_HERE_M("Work in progress");
    }
}

static instanceOop makeMemberName(int refKind, Method *target, instanceOop nameObj, instanceOop typeObj) {
    auto newObj = nameObj->getInstanceClass()->newInstance();
    int newFlags = (target->getAccessFlag() & (~ACC_ANNOTATION)) | 0x10000 | (refKind << 24);

    if (target->checkAnnotation(L"Lsun/reflect/CallerSensitive;")) {
        // copied from hotspot vm
        newFlags |= 0x100000;
    }

    if (refKind < REF_invokeVirtual || refKind > REF_invokeInterface) {
        SHOULD_NOT_REACH_HERE();
    }

    newObj->setFieldValue(MEMBER_NAME, L"name", L"Ljava/lang/String;",
        java::lang::String::intern(target->getName()));
    newObj->setFieldValue(MEMBER_NAME, L"clazz", L"Ljava/lang/Class;",
        target->getClass()->getJavaMirror());
    newObj->setFieldValue(MEMBER_NAME, L"type", L"Ljava/lang/Object;", typeObj);
    newObj->setFieldValue(MEMBER_NAME, L"flags", L"I", new intOopDesc(newFlags));

    LockGuard lg(MEMBER_NAME_CACHE_LOCK);
    MEMBER_NAME_CACHE.insert(newObj);
    return newObj;
}

static instanceOop makeMemberName(int refKind, Field *target, instanceOop nameObj) {
    auto newObj = nameObj->getInstanceClass()->newInstance();

    int newFlags = (target->getAccessFlag() & (~ACC_ANNOTATION));
    if (target->isStatic()) {
        newFlags |= 0x40000 | (REF_getStatic << 24);
    } else {
        newFlags |= 0x40000 | (REF_getField << 24);
    }

    if (refKind > REF_getStatic) {
        newFlags += ((REF_putField - 1) << 24);
    }

    newObj->setFieldValue(MEMBER_NAME, L"name", L"Ljava/lang/String;",
        java::lang::String::intern(target->getName()));
    newObj->setFieldValue(MEMBER_NAME, L"clazz", L"Ljava/lang/Class;",
        target->getClass()->getJavaMirror());
    newObj->setFieldValue(MEMBER_NAME, L"type", L"Ljava/lang/Object;",
        java::lang::String::intern(target->getDescriptor()));
    newObj->setFieldValue(MEMBER_NAME, L"flags", L"I", new intOopDesc(newFlags));

    LockGuard lg(MEMBER_NAME_CACHE_LOCK);
    MEMBER_NAME_CACHE.insert(newObj);
    return newObj;
}

JAVA_NATIVE void Java_java_lang_invoke_MethodHandleNatives_registerNatives(JNIEnv *env, jclass mhn) {
    D("java/lang/MethodHandleNatives.registerNatives()V");
}

JAVA_NATIVE jint Java_java_lang_invoke_MethodHandleNatives_getConstant(JNIEnv *env, jclass mhn, jint i) {
    // Copied from hotspot vm
    assert(i == 4);
    return 0;
}

JAVA_NATIVE jobject Java_java_lang_invoke_MethodHandleNatives_resolve(JNIEnv *env, jclass mhn,
                                                                      jobject jmemberName, jclass caller) {
    auto memberName = Resolver::instance(jmemberName);
    if (memberName == nullptr) {
        SHOULD_NOT_REACH_HERE();
    }

    oop object;
    if (!memberName->getFieldValue(MEMBER_NAME, L"clazz", L"Ljava/lang/Class;", &object)) {
        SHOULD_NOT_REACH_HERE();
    }
    auto mirrorObj = Resolver::mirror(object);

    InstanceKlass *klass = nullptr;
    auto _klass = mirrorObj->getTarget();
    if (_klass == nullptr) {
        SHOULD_NOT_REACH_HERE();
    }

    switch (_klass->getClassType()) {
        case ClassType::INSTANCE_CLASS:
            klass = (InstanceKlass *) _klass;
            break;
        case ClassType::TYPE_ARRAY_CLASS:
        case ClassType::OBJECT_ARRAY_CLASS:
            klass = Global::_Object;
            break;
        default:
            SHOULD_NOT_REACH_HERE();
    }

    if (!memberName->getFieldValue(MEMBER_NAME, L"name", L"Ljava/lang/String;", &object)) {
        SHOULD_NOT_REACH_HERE();
    }
    auto nameObj = Resolver::instance(object);
    const auto &name = java::lang::String::toNativeString(nameObj);
    if (name == L"<init>" || name == L"<clinit>") {
        SHOULD_NOT_REACH_HERE();
    }

    if (!memberName->getFieldValue(MEMBER_NAME, L"type", L"Ljava/lang/Object;", &object)) {
        SHOULD_NOT_REACH_HERE();
    }
    auto typeObj = Resolver::instance(object);

    if (!memberName->getFieldValue(MEMBER_NAME, L"flags", L"I", &object)) {
        SHOULD_NOT_REACH_HERE();
    }

    auto flagsObj = intOop(object);
    auto flags = (unsigned int) flagsObj->getValue();

    // copied from MemberName.java:
    unsigned int refKind = (flags >> 24) & 15;
    const auto descriptor = resolveDescriptor(klass, name, typeObj);

    if (isMethod(flags)) {
        auto target = resolveTargetMethod(klass, refKind, name, descriptor);
        if (target == nullptr) {
            auto thread = Threads::currentThread();
            if (thread == nullptr) {
                SHOULD_NOT_REACH_HERE();
            }

            auto ex = (InstanceKlass *) BootstrapClassLoader::get()->loadClass(L"java/lang/LinkageError");
            thread->throwException(ex, false);
            return nullptr;
        }

        return makeMemberName(refKind, target, memberName, typeObj);
    }

    if (isField(flags)) {
        auto fieldInfo = klass->getStaticFieldInfo(klass->getName(), name, descriptor);
        if (fieldInfo == nullptr) {
            fieldInfo = klass->getInstanceFieldInfo(klass->getName(), name, descriptor);
        }

        if (fieldInfo == nullptr) {
            SHOULD_NOT_REACH_HERE();
        }

        return makeMemberName(refKind, fieldInfo->_field, memberName);
    }

    if (isConstructor(flags)) {
        // TODO: support constructor
    }

    SHOULD_NOT_REACH_HERE_M("Work in progress");
}


