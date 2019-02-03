//
// Created by kiva on 2018/3/27.
//
#include <kivm/bytecode/execution.h>
#include <kivm/bytecode/javaCall.h>
#include <kivm/oop/instanceOop.h>
#include <kivm/oop/primitiveOop.h>
#include <kivm/oop/arrayOop.h>
#include <kivm/oop/method.h>

namespace kivm {
    static bool checkInherit(Klass *S, Klass *T) {
        auto super = S;
        do {
            if (super == T) {
                return true;
            }
        } while ((super = super->getSuperClass()) != nullptr);
        return false;
    }

    bool Execution::initializeClass(JavaThread *thread, InstanceKlass *klass) {
        if (klass->getClassState() == ClassState::LINKED) {
            klass->setClassState(ClassState::BEING_INITIALIZED);
            D("Initializing class %s",
                strings::toStdString(klass->getName()).c_str());

            // Initialize super classes first.
            Klass *super_klass = klass->getSuperClass();
            if (super_klass != nullptr) {
                if (!Execution::initializeClass(thread, (InstanceKlass *) super_klass)) {
                    return false;
                }
            }

            klass->initClass();
            auto *clinit = klass->getThisClassMethod(L"<clinit>", L"()V");
            if (clinit != nullptr && clinit->getClass() == klass) {
                D("<clinit> found in %s, invoking.",
                    strings::toStdString(klass->getName()).c_str());
                JavaCall::withArgs(thread, clinit, {});
                if (thread->isExceptionOccurred()) {
                    return false;
                }
            }
            klass->setClassState(ClassState::FULLY_INITIALIZED);
        }
        return true;
    }

    void Execution::loadConstant(RuntimeConstantPool *rt, Stack &stack, int constantIndex) {
        switch (rt->getConstantTag(constantIndex)) {
            case CONSTANT_Integer: {
                stack.pushInt(rt->getInt(constantIndex));
                break;
            }
            case CONSTANT_Float: {
                stack.pushFloat(rt->getFloat(constantIndex));
                break;
            }
            case CONSTANT_Long: {
                stack.pushLong(rt->getLong(constantIndex));
                break;
            }
            case CONSTANT_Double: {
                stack.pushDouble(rt->getDouble(constantIndex));
                break;
            }
            case CONSTANT_String: {
                stack.pushReference(rt->getString(constantIndex));
                break;
            }
            case CONSTANT_Class: {
                Klass *klass = rt->getClass(constantIndex);
                auto mirror = klass->getJavaMirror();
                if (mirror == nullptr) {
                    SHOULD_NOT_REACH_HERE_M("Pushing null classes");
                }
                stack.pushReference(mirror);
                break;
            }
            default: {
                SHOULD_NOT_REACH_HERE_M("Unsupported constant tag");
                break;
            }
        }
    }

    void Execution::instanceOf(JavaThread *thread, RuntimeConstantPool *rt,
                               Stack &stack, int constantIndex,
                               bool checkCast) {
        jobject ref = stack.popReference();
        if (ref == nullptr) {
            if (checkCast) {
                stack.pushReference(ref);
                return;
            }
            stack.pushInt(0);
            D("null instanceof <ref>: always false");
            return;
        }

        oop obj = Resolver::javaOop(ref);
        Klass *objClass = obj->getClass();
        Klass *targetClass = rt->getClass(constantIndex);

        bool result = Execution::instanceOf(objClass, targetClass);
        D("Execution::instanceOf: %s %s %s: %s",
            strings::toStdString(objClass->getName()).c_str(),
            checkCast ? "checkcast" : "instanceof",
            strings::toStdString(targetClass->getName()).c_str(),
            result ? "true" : "false");

        if (result) {
            if (checkCast) {
                stack.pushReference(ref);
            } else {
                stack.pushInt(1);
            }
        } else {
            if (checkCast) {
                auto klass = (InstanceKlass *) BootstrapClassLoader::get()
                    ->loadClass(L"java/lang/ClassCastException");
                thread->throwException(klass,
                    strings::replaceAll(objClass->getName(),
                        Global::SLASH, Global::DOT)
                    + L" cannot be cast to "
                    + strings::replaceAll(targetClass->getName(),
                        Global::SLASH, Global::DOT),
                    false);
            } else {
                stack.pushInt(0);
            }
        }
    }

    bool Execution::instanceOf(Klass *S, Klass *T) {
        // Return if S and T are the same class
        if (S == T) {
            return true;
        }

        // If S is an ordinary (non-array) class
        if (S->getClassType() == ClassType::INSTANCE_CLASS
            && !S->isInterface()) {

            // If T is an interface type, then S must implement interface T.
            if (T->isInterface()) {
                auto instanceKlassS = (InstanceKlass *) S;
                return instanceKlassS->checkInterface((InstanceKlass *) T);
            }

            // If T is a class type, then S must be the same class as T,
            // or S must be a subclass of T;
            if (T->getClassType() == ClassType::INSTANCE_CLASS) {
                return checkInherit(S, T);
            }

            return false;
        }

        // If S is an interface type
        if (S->isInterface()) {
            // If T is an interface type, then T must be the same interface as S
            // or a superinterface of S.
            if (T->isInterface()) {
                return checkInherit(S, T);
            }

            // If T is a class type, then T must be Object
            if (T->getClassType() == ClassType::INSTANCE_CLASS) {
                return T == Global::_Object;
            }
            return false;
        }

        // If S is a class representing the array type SC[],
        // that is, an array of components of type SC
        if (S->getClassType() == ClassType::TYPE_ARRAY_CLASS
            || S->getClassType() == ClassType::OBJECT_ARRAY_CLASS) {

            // If T is an interface type, then T must be one of the interfaces
            // implemented by arrays (JLS §4.10.3).
            // https://docs.oracle.com/javase/specs/jls/se7/html/jls-4.html#jls-4.10.3
            // array implements:
            // 1. java/lang/Cloneable
            // 2. java/io/Serializable
            if (T->isInterface()) {
                return T == Global::_Serializable
                       || T == Global::_Cloneable;
            }

            // If T is a class type, then T must be Object
            if (T->getClassType() == ClassType::INSTANCE_CLASS) {
                return T == Global::_Object;
            }

            // If T is an array type TC[], that is, an array of components of type TC
            // then one of the following must be true

            // If TC and SC are the same primitive type
            if (T->getClassType() == ClassType::TYPE_ARRAY_CLASS
                && S->getClassType() == ClassType::TYPE_ARRAY_CLASS) {
                auto typeArrayKlassS = (TypeArrayKlass *) S;
                auto typeArrayKlassT = (TypeArrayKlass *) T;
                return typeArrayKlassS->getDimension() == typeArrayKlassT->getDimension()
                       && typeArrayKlassS->getComponentType() == typeArrayKlassT->getComponentType();
            }

            // TC and SC are reference types, and type SC can be cast to TC by these run-time rules.
            if (T->getClassType() == ClassType::OBJECT_ARRAY_CLASS
                && S->getClassType() == ClassType::OBJECT_ARRAY_CLASS) {
                auto objectArrayKlassS = (ObjectArrayKlass *) S;
                auto objectArrayKlassT = (ObjectArrayKlass *) T;
                return objectArrayKlassS->getDimension() == objectArrayKlassT->getDimension()
                       && checkInherit(objectArrayKlassS->getComponentType(),
                    objectArrayKlassT->getComponentType());
            }
            return false;
        }
        return false;
    }

    void Execution::getField(JavaThread *thread, RuntimeConstantPool *rt, instanceOop receiver, Stack &stack,
                             int constantIndex) {
        bool isStatic = receiver == nullptr;
        auto field = isStatic
                     ? rt->getStaticField(constantIndex)
                     : rt->getInstanceField(constantIndex);

        if (field == nullptr) {
            // TODO: NoSuchFieldException
            PANIC("FieldID is null, constantIndex: %d", constantIndex);
        }

        auto instanceKlass = field->_field->getClass();
        if (!Execution::initializeClass(thread, instanceKlass)) {
            return;
        }

        oop fieldValue = nullptr;

        // We are getting a static field.
        if (receiver == nullptr) {
            if (!instanceKlass->getStaticFieldValue(field, &fieldValue)) {
                PANIC("Cannot get static field value, constantIndex: %d", constantIndex);
            }
        } else {
            // We are getting an instance field.
            if (!instanceKlass->getInstanceFieldValue(receiver, field, &fieldValue)) {
                PANIC("Cannot get instance field value, constantIndex: %d", constantIndex);
            }
        }

        switch (field->_field->getValueType()) {
            case ValueType::OBJECT:
            case ValueType::ARRAY: {
                stack.pushReference(fieldValue);
                break;
            }

            case ValueType::INT:
            case ValueType::SHORT:
            case ValueType::CHAR:
            case ValueType::BOOLEAN:
            case ValueType::BYTE: {
                auto intObject = (intOop) fieldValue;
                stack.pushInt(intObject->getValue());
                break;
            }

            case ValueType::FLOAT: {
                auto floatObject = (floatOop) fieldValue;
                stack.pushFloat(floatObject->getValue());
                break;
            }

            case ValueType::DOUBLE: {
                auto doubleObject = (doubleOop) fieldValue;
                stack.pushDouble(doubleObject->getValue());
                break;
            }

            case ValueType::LONG: {
                auto longObject = (longOop) fieldValue;
                stack.pushLong(longObject->getValue());
                break;
            }

            case ValueType::VOID:
            default:
                SHOULD_NOT_REACH_HERE();
                break;
        }
    }

#define PUTFIELD(value) \
        if (isStatic) { \
            instanceKlass->setStaticFieldValue(field, value); \
        } else { \
            jobject receiverRef = stack.popReference(); \
            instanceOop receiver = Resolver::instance(receiverRef); \
            if (receiver == nullptr) { \
                thread->throwException(Global::_NullPointerException, false); \
            } else { \
                instanceKlass->setInstanceFieldValue(receiver, field, value); \
            } \
        }

    void Execution::putField(JavaThread *thread, RuntimeConstantPool *rt, Stack &stack,
                             int constantIndex, bool isStatic) {
        auto field = isStatic
                     ? rt->getStaticField(constantIndex)
                     : rt->getInstanceField(constantIndex);

        if (field == nullptr) {
            // TODO: NoSuchFieldException
            PANIC("FieldID is null, constantIndex: %d", constantIndex);
        }

        auto instanceKlass = field->_field->getClass();
        if (!Execution::initializeClass(thread, instanceKlass)) {
            return;
        }

        assert(isStatic == field->_field->isStatic());

        switch (field->_field->getValueType()) {
            case ValueType::OBJECT:
            case ValueType::ARRAY: {
                jobject ref = stack.popReference();
                oop value = Resolver::javaOop(ref);
                PUTFIELD(value);
                break;
            }

            case ValueType::INT:
            case ValueType::SHORT:
            case ValueType::CHAR:
            case ValueType::BOOLEAN:
            case ValueType::BYTE: {
                auto intObject = new intOopDesc(stack.popInt());
                PUTFIELD(intObject);
                break;
            }

            case ValueType::FLOAT: {
                auto floatObject = new floatOopDesc(stack.popFloat());
                PUTFIELD(floatObject);
                break;
            }

            case ValueType::DOUBLE: {
                auto doubleObject = new doubleOopDesc(stack.popDouble());
                PUTFIELD(doubleObject);
                break;
            }

            case ValueType::LONG: {
                auto longObject = new longOopDesc(stack.popLong());
                PUTFIELD(longObject);
                break;
            }

            case ValueType::VOID:
            default:
                SHOULD_NOT_REACH_HERE();
                break;
        }
    }

    instanceOop Execution::newInstance(JavaThread *thread, RuntimeConstantPool *rt, int constantIndex) {
        auto klass = rt->getClass(constantIndex);
        if (klass == nullptr) {
            PANIC("Cannot get class info from constant pool");
        }

        if (klass->getClassType() != ClassType::INSTANCE_CLASS) {
            SHOULD_NOT_REACH_HERE_M("Not an instance class");
        }

        auto instanceKlass = (InstanceKlass *) klass;
        if (!Execution::initializeClass(thread, instanceKlass)) {
            return nullptr;
        }
        return instanceKlass->newInstance();
    }

    typeArrayOop Execution::newPrimitiveArray(JavaThread *thread, int arrayType, int length) {
        Klass *arrayClass = nullptr;

        switch (arrayType) {
            case T_BOOLEAN:
                arrayClass = SystemDictionary::get()->find(L"[Z");
                break;
            case T_BYTE:
                arrayClass = SystemDictionary::get()->find(L"[B");
                break;
            case T_CHAR:
                arrayClass = SystemDictionary::get()->find(L"[C");
                break;
            case T_DOUBLE:
                arrayClass = SystemDictionary::get()->find(L"[D");
                break;
            case T_FLOAT:
                arrayClass = SystemDictionary::get()->find(L"[F");
                break;
            case T_INT:
                arrayClass = SystemDictionary::get()->find(L"[I");
                break;
            case T_SHORT:
                arrayClass = SystemDictionary::get()->find(L"[S");
                break;
            case T_LONG:
                arrayClass = SystemDictionary::get()->find(L"[J");
                break;
            default:
                arrayClass = nullptr;
                break;
        }

        if (arrayClass == nullptr || arrayClass->getClassType() != ClassType::TYPE_ARRAY_CLASS) {
            PANIC("Unrecognized array type: %d, array class: %p", arrayType, arrayClass);
        }

        auto typeArrayClass = (TypeArrayKlass *) arrayClass;
        return typeArrayClass->newInstance(length);
    }

    objectArrayOop Execution::newObjectArray(JavaThread *thread, RuntimeConstantPool *rt,
                                             int constantIndex, int length) {
        Klass *arrayClass = rt->getClass(constantIndex);
        if (arrayClass == nullptr) {
            PANIC("Unrecognized array type(in constant pool): %d, array class: %p", constantIndex, arrayClass);
        }

        ClassType classType = arrayClass->getClassType();
        ObjectArrayKlass *objectArrayKlass = nullptr;

        if (classType == ClassType::INSTANCE_CLASS) {
            // ClassType::INSTANCE_CLASS
            auto instanceKlass = (InstanceKlass *) arrayClass;
            if (!Execution::initializeClass(thread, instanceKlass)) {
                return nullptr;
            }

            ClassLoader *classLoader = instanceKlass->getClassLoader();
            if (classLoader == nullptr) {
                objectArrayKlass = (ObjectArrayKlass *) BootstrapClassLoader::get()
                    ->loadClass(L"[L" + instanceKlass->getName() + L";");
            } else {
                objectArrayKlass = (ObjectArrayKlass *) classLoader
                    ->loadClass(L"[L" + instanceKlass->getName() + L";");
            }

        } else if (classType == ClassType::OBJECT_ARRAY_CLASS) {
            // ClassType::OBJECT_ARRAY_CLASS
            auto wrapperClass = (ObjectArrayKlass *) arrayClass;
            ClassLoader *classLoader = wrapperClass->getComponentType()->getClassLoader();

            if (classLoader == nullptr) {
                objectArrayKlass = (ObjectArrayKlass *) BootstrapClassLoader::get()
                    ->loadClass(L"[" + wrapperClass->getName() + L";");
            } else {
                objectArrayKlass = (ObjectArrayKlass *) classLoader
                    ->loadClass(L"[" + wrapperClass->getName() + L";");
            }

        } else if (classType == ClassType::TYPE_ARRAY_CLASS) {
            // ClassType::TYPE_ARRAY_CLASS
            auto typeArrayKlass = (TypeArrayKlass *) arrayClass;

            objectArrayKlass = (ObjectArrayKlass *) BootstrapClassLoader::get()
                ->loadClass(L"[" + typeArrayKlass->getName());

        } else {
            PANIC("Unrecognized class type");
        }

        if (objectArrayKlass == nullptr) {
            PANIC("Cannot get component type of an object array");
        }

        return objectArrayKlass->newInstance(length);
    }

    static arrayOop newMultiObjectArrayHelper(ArrayKlass *arrayKlass,
                                              const std::deque<int> &length,
                                              int lengthIndex);

    arrayOop Execution::newMultiObjectArray(JavaThread *thread, RuntimeConstantPool *rt, int constantIndex,
                                            int dimension, const std::deque<int> &length) {
        if (length.size() != dimension) {
            PANIC("some sub arrays cannot be created due to lack of length");
        }

        auto klass = rt->getClass(constantIndex);
        ClassType classType = klass->getClassType();

        ArrayKlass *arrayKlass = nullptr;

        if (classType == ClassType::INSTANCE_CLASS) {
            // XXX: What does this mean?
            // The JVM Specification says this could be an instance class
            // but HotSpot says this must be array class
            PANIC("wtf");

        } else if (classType == ClassType::OBJECT_ARRAY_CLASS) {
            arrayKlass = (ArrayKlass *) klass;

        } else if (classType == ClassType::TYPE_ARRAY_CLASS) {
            arrayKlass = (TypeArrayKlass *) klass;
        }

        if (arrayKlass == nullptr || arrayKlass->getDimension() != dimension) {
            PANIC("invalid dimension");
        }

        return newMultiObjectArrayHelper(arrayKlass, length, 0);
    }

    static arrayOop newMultiObjectArrayHelper(ArrayKlass *arrayKlass,
                                              const std::deque<int> &length,
                                              int lengthIndex) {
        int arrayLength = length[lengthIndex];
        arrayOop array = nullptr;
        ArrayKlass *downDimensionType = nullptr;

        if (arrayKlass->getClassType() == ClassType::TYPE_ARRAY_CLASS) {
            auto typeArrayKlass = (TypeArrayKlass *) arrayKlass;
            downDimensionType = typeArrayKlass->getDownDimensionType();
            array = typeArrayKlass->newInstance(arrayLength);

        } else if (arrayKlass->getClassType() == ClassType::OBJECT_ARRAY_CLASS) {
            auto objectArrayKlass = (ObjectArrayKlass *) arrayKlass;
            downDimensionType = objectArrayKlass->getDownDimensionType();
            array = objectArrayKlass->newInstance(arrayLength);

        } else {
            PANIC("Expected type array or object array");
        }

        if (array == nullptr) {
            PANIC("Failed to allocate array");
        }

        if (lengthIndex < length.size() - 1) {
            if (downDimensionType == nullptr) {
                PANIC("downDimensionType is null, expected non-null to create subarrays");
            }
            for (int i = 0; i < array->getLength(); ++i) {
                arrayOop elementArray = newMultiObjectArrayHelper(downDimensionType, length, lengthIndex + 1);
                array->setElementAt(i, elementArray);
            }

        } else {
            // arrayKlass->getDimension() must be 1 as we have met the last dimension
            if (arrayKlass->getDimension() != 1) {
                PANIC("Expected the last dimension to be 1");
            }
        }

        return array;
    }
}
