//
// Created by kiva on 2018/3/27.
//
#include <kivm/bytecode/execution.h>
#include <kivm/bytecode/invocationContext.h>
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

    void Execution::initializeClass(JavaThread *javaThread, InstanceKlass *klass) {
        if (klass->getClassState() == ClassState::LINKED) {
            klass->setClassState(ClassState::BEING_INITIALIZED);
            D("Initializing class %s",
              strings::toStdString(klass->getName()).c_str());

            // Initialize super classes first.
            Klass *super_klass = klass->getSuperClass();
            if (super_klass != nullptr) {
                Execution::initializeClass(javaThread, (InstanceKlass *) super_klass);
            }

            klass->initClass();
            auto *clinit = klass->getThisClassMethod(L"<clinit>", L"()V");
            if (clinit != nullptr && clinit->getClass() == klass) {
                D("<clinit> found in %s, invoking.",
                  strings::toStdString(klass->getName()).c_str());
                javaThread->runMethod(clinit, {});
            }
            klass->setClassState(ClassState::FULLY_INITIALIZED);
        }
    }

    void Execution::callDefaultConstructor(JavaThread *javaThread, instanceOop oop) {
        auto klass = (InstanceKlass *) oop->getClass();
        auto ctor = klass->getThisClassMethod(L"<init>", L"()V");
        assert(ctor != nullptr);
        javaThread->runMethod(ctor, {oop});
    }

    void Execution::callVoidMethod(JavaThread *javaThread, Method *method, const std::list<oop> &args) {
        javaThread->runMethod(method, args);
    }

    void Execution::loadConstant(RuntimeConstantPool *rt, Stack &stack, int constantIndex) {
        D("constant index: %d, tag: %d", constantIndex, rt->getConstantTag(constantIndex));
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
                    PANIC("Pushing null classes");
                }
                stack.pushReference(mirror);
                break;
            }
            default: {
                PANIC("Unsupported constant tag");
                break;
            }
        }
    }

    void Execution::instanceOf(RuntimeConstantPool *rt, Stack &stack, int constantIndex, bool checkCast) {
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

        oop obj = Resolver::resolveJObject(ref);
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
                // TODO: throw java.lang.ClassCastException
                PANIC("java.lang.ClassCastException");
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
                return T == Global::java_lang_Object;
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
                return T == Global::java_lang_Serializable
                       || T == Global::java_lang_Cloneable;
            }

            // If T is a class type, then T must be Object
            if (T->getClassType() == ClassType::INSTANCE_CLASS) {
                return T == Global::java_lang_Object;
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

    void Execution::loadIntArrayElement(Stack &stack) {
        int index = stack.popInt();
        jobject ref = stack.popReference();
        if (ref == nullptr) {
            // TODO: throw NullPointerException
            PANIC("java.lang.NullPointerException");
        }
        auto array = Resolver::resolveTypeArray(ref);
        if (array == nullptr) {
            PANIC("not a type array");
        }

        auto element = (intOop) array->getElementAt(index);
        stack.pushInt(element->getValue());
    }

    void Execution::loadFloatArrayElement(Stack &stack) {
        int index = stack.popInt();
        jobject ref = stack.popReference();
        if (ref == nullptr) {
            // TODO: throw NullPointerException
            PANIC("java.lang.NullPointerException");
        }
        auto array = Resolver::resolveTypeArray(ref);
        if (array == nullptr) {
            PANIC("not a type array");
        }

        auto element = (floatOop) array->getElementAt(index);
        stack.pushFloat(element->getValue());
    }

    void Execution::loadDoubleArrayElement(Stack &stack) {
        int index = stack.popInt();
        jobject ref = stack.popReference();
        if (ref == nullptr) {
            // TODO: throw NullPointerException
            PANIC("java.lang.NullPointerException");
        }
        auto array = Resolver::resolveTypeArray(ref);
        if (array == nullptr) {
            PANIC("not a type array");
        }

        auto element = (doubleOop) array->getElementAt(index);
        stack.pushDouble(element->getValue());
    }

    void Execution::loadLongArrayElement(Stack &stack) {
        int index = stack.popInt();
        jobject ref = stack.popReference();
        if (ref == nullptr) {
            // TODO: throw NullPointerException
            PANIC("java.lang.NullPointerException");
        }
        auto array = Resolver::resolveTypeArray(ref);
        if (array == nullptr) {
            PANIC("not a type array");
        }

        auto element = (longOop) array->getElementAt(index);
        stack.pushLong(element->getValue());
    }

    void Execution::loadObjectArrayElement(Stack &stack) {
        int index = stack.popInt();
        jobject ref = stack.popReference();
        if (ref == nullptr) {
            // TODO: throw NullPointerException
            PANIC("java.lang.NullPointerException");
        }

        auto array = Resolver::resolveObjectArray(ref);
        if (array == nullptr) {
            PANIC("not an object array");
        }

        stack.pushReference(array->getElementAt(index));
    }

    void Execution::storeIntArrayElement(Stack &stack) {
        jint value = stack.popInt();
        int index = stack.popInt();
        jobject ref = stack.popReference();
        if (ref == nullptr) {
            // TODO: throw NullPointerException
            PANIC("java.lang.NullPointerException");
        }
        auto array = Resolver::resolveTypeArray(ref);
        if (array == nullptr) {
            PANIC("not a type array");
        }

        array->setElementAt(index, new intOopDesc(value));
    }

    void Execution::storeFloatArrayElement(Stack &stack) {
        jfloat value = stack.popFloat();
        int index = stack.popInt();
        jobject ref = stack.popReference();
        if (ref == nullptr) {
            // TODO: throw NullPointerException
            PANIC("java.lang.NullPointerException");
        }
        auto array = Resolver::resolveTypeArray(ref);
        if (array == nullptr) {
            PANIC("not a type array");
        }

        array->setElementAt(index, new floatOopDesc(value));
    }

    void Execution::storeDoubleArrayElement(Stack &stack) {
        jdouble value = stack.popDouble();
        int index = stack.popInt();
        jobject ref = stack.popReference();
        if (ref == nullptr) {
            // TODO: throw NullPointerException
            PANIC("java.lang.NullPointerException");
        }
        auto array = Resolver::resolveTypeArray(ref);
        if (array == nullptr) {
            PANIC("not a type array");
        }

        array->setElementAt(index, new doubleOopDesc(value));
    }

    void Execution::storeLongArrayElement(Stack &stack) {
        jlong value = stack.popLong();
        int index = stack.popInt();
        jobject ref = stack.popReference();
        if (ref == nullptr) {
            // TODO: throw NullPointerException
            PANIC("java.lang.NullPointerException");
        }
        auto array = Resolver::resolveTypeArray(ref);
        if (array == nullptr) {
            PANIC("not a type array");
        }

        array->setElementAt(index, new longOopDesc(value));
    }

    void Execution::storeObjectArrayElement(Stack &stack) {
        jobject value = stack.popReference();
        int index = stack.popInt();
        jobject ref = stack.popReference();
        if (ref == nullptr) {
            // TODO: throw NullPointerException
            PANIC("java.lang.NullPointerException");
        }
        auto array = Resolver::resolveObjectArray(ref);
        if (array == nullptr) {
            PANIC("not an object array");
        }

        array->setElementAt(index, Resolver::resolveJObject(value));
    }

    void Execution::getField(JavaThread *thread, RuntimeConstantPool *rt, instanceOop receiver, Stack &stack,
                             int constantIndex) {
        auto field = rt->getField(constantIndex);
        if (field == nullptr) {
            PANIC("FieldID is null, constantIndex: %d", constantIndex);
        }

        auto instanceKlass = field->_field->getClass();
        Execution::initializeClass(thread, instanceKlass);

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
                PANIC("Field cannot be typed void");
                break;
            default:
                PANIC("Unrecognized field value type");
                break;
        }
    }

    void Execution::putField(JavaThread *thread, RuntimeConstantPool *rt, Stack &stack, int constantIndex) {
        auto field = rt->getField(constantIndex);
        if (field == nullptr) {
            PANIC("FieldID is null, constantIndex: %d", constantIndex);
        }

        auto instanceKlass = field->_field->getClass();
        Execution::initializeClass(thread, instanceKlass);

        bool isStatic = field->_field->isStatic();

#define PUTFIELD(value) \
        if (isStatic) { \
            instanceKlass->setStaticFieldValue(field, value); \
        } else { \
            jobject receiverRef = stack.popReference(); \
            if (receiverRef == nullptr) { \
                PANIC("java.lang.NullPointerException"); \
            } \
            instanceOop receiver = Resolver::resolveInstance(receiverRef); \
            if (receiver == nullptr) { \
                PANIC("Not an instance oop"); \
            } \
            instanceKlass->setInstanceFieldValue(receiver, field, value); \
        }

        switch (field->_field->getValueType()) {
            case ValueType::OBJECT:
            case ValueType::ARRAY: {
                jobject ref = stack.popReference();
                oop value = Resolver::resolveJObject(ref);
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
                PANIC("Field cannot be typed void");
                break;
            default:
                PANIC("Unrecognized field value type");
                break;
        }
    }

    instanceOop Execution::newInstance(JavaThread *thread, RuntimeConstantPool *rt, int constantIndex) {
        auto klass = rt->getClass(constantIndex);
        if (klass == nullptr) {
            PANIC("Cannot get class info from constant pool");
        }

        if (klass->getClassType() != ClassType::INSTANCE_CLASS) {
            PANIC("Not an instance class");
        }

        auto instanceKlass = (InstanceKlass *) klass;
        Execution::initializeClass(thread, instanceKlass);
        return instanceKlass->newInstance();
    }

    typeArrayOop Execution::newPrimitiveArray(JavaThread *thread, int arrayType, int length) {
        if (length < 0) {
            // TODO: NegativeArraySizeException
            PANIC("java.lang.NegativeArraySizeException");
        }

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
        if (length < 0) {
            // TODO: NegativeArraySizeException
            PANIC("java.lang.NegativeArraySizeException");
        }

        Klass *arrayClass = rt->getClass(constantIndex);
        if (arrayClass == nullptr) {
            PANIC("Unrecognized array type(in constant pool): %d, array class: %p", constantIndex, arrayClass);
        }

        ClassType classType = arrayClass->getClassType();
        ObjectArrayKlass *objectArrayKlass = nullptr;

        if (classType == ClassType::INSTANCE_CLASS) {
            // ClassType::INSTANCE_CLASS
            auto instanceKlass = (InstanceKlass *) arrayClass;
            Execution::initializeClass(thread, instanceKlass);

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
