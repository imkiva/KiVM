//
// Created by kiva on 2018/3/27.
//
#include <kivm/bytecode/execution.h>
#include <kivm/oop/instanceOop.h>
#include <kivm/oop/primitiveOop.h>
#include <kivm/oop/arrayOop.h>
#include <kivm/method.h>

namespace kivm {
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

            auto *clinit = klass->getVirtualMethod(L"<clinit>", L"()V");
            if (clinit != nullptr && clinit->getClass() == klass) {
                D("<clinit> found in %s, invoking.",
                  strings::toStdString(klass->getName()).c_str());
                javaThread->runMethod(clinit, {});
            } else {
                D("<clinit> not found in %s, skipping.",
                  strings::toStdString(klass->getName()).c_str());
            }
            klass->setClassState(ClassState::FULLY_INITIALIZED);
        }
    }

    void Execution::callDefaultConstructor(JavaThread *javaThread, instanceOop oop) {
        auto klass = (InstanceKlass *) oop->getClass();
        auto default_init = klass->getVirtualMethod(L"<init>", L"()V");
        assert(default_init != nullptr);
        javaThread->runMethod(default_init, {oop});
    }

    void Execution::callVoidMethod(JavaThread *javaThread, Method *method, const std::list<oop> &args) {
        javaThread->runMethod(method, args);
    }

    void Execution::loadConstant(RuntimeConstantPool *rt, Stack &stack, int constantIndex) {
        D("constant index: %d, tag: %d", constantIndex, rt->getConstantTag(constantIndex));
        switch (rt->getConstantTag(constantIndex)) {
            case CONSTANT_Integer: {
                D("load: CONSTANT_Integer");
                stack.pushInt(rt->getInt(constantIndex));
                break;
            }
            case CONSTANT_Float: {
                D("load: CONSTANT_Float");
                stack.pushFloat(rt->getFloat(constantIndex));
                break;
            }
            case CONSTANT_Long: {
                D("load: CONSTANT_Long");
                stack.pushLong(rt->getLong(constantIndex));
                break;
            }
            case CONSTANT_Double: {
                D("load: CONSTANT_Double");
                stack.pushDouble(rt->getDouble(constantIndex));
                break;
            }
            case CONSTANT_String: {
                D("load: CONSTANT_String");
                stack.pushReference(rt->getString(constantIndex));
                break;
            }
            case CONSTANT_Class: {
                D("load: CONSTANT_Class");
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

    bool Execution::instanceOf(Klass *ref, Klass *klass) {
        return false;
    }

    void Execution::loadIntArrayElement(Stack &stack) {
        int index = stack.popInt();
        jobject ref = stack.popReference();
        if (ref == nullptr) {
            // TODO: throw NullPointerException
            PANIC("java.lang.NullPointerException");
        }
        auto array = Resolver::tryResolveTypeArray(ref);
        if (array == nullptr) {
            // TODO: throw ClassCastException
            PANIC("java.lang.ClassCastException");
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
        auto array = Resolver::tryResolveTypeArray(ref);
        if (array == nullptr) {
            // TODO: throw ClassCastException
            PANIC("java.lang.ClassCastException");
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
        auto array = Resolver::tryResolveTypeArray(ref);
        if (array == nullptr) {
            // TODO: throw ClassCastException
            PANIC("java.lang.ClassCastException");
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
        auto array = Resolver::tryResolveTypeArray(ref);
        if (array == nullptr) {
            // TODO: throw ClassCastException
            PANIC("java.lang.ClassCastException");
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

        auto array = Resolver::tryResolveObjectArray(ref);
        if (array == nullptr) {
            // TODO: throw ClassCastException
            PANIC("java.lang.ClassCastException");
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
        auto array = Resolver::tryResolveTypeArray(ref);
        if (array == nullptr) {
            // TODO: throw ClassCastException
            PANIC("java.lang.ClassCastException");
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
        auto array = Resolver::tryResolveTypeArray(ref);
        if (array == nullptr) {
            // TODO: throw ClassCastException
            PANIC("java.lang.ClassCastException");
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
        auto array = Resolver::tryResolveTypeArray(ref);
        if (array == nullptr) {
            // TODO: throw ClassCastException
            PANIC("java.lang.ClassCastException");
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
        auto array = Resolver::tryResolveTypeArray(ref);
        if (array == nullptr) {
            // TODO: throw ClassCastException
            PANIC("java.lang.ClassCastException");
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
        auto array = Resolver::tryResolveTypeArray(ref);
        if (array == nullptr) {
            // TODO: throw ClassCastException
            PANIC("java.lang.ClassCastException");
        }

        array->setElementAt(index, Resolver::resolveJObject(value));
    }

    void Execution::getField(InstanceKlass *instanceKlass, instanceOop receiver, Stack &stack, int constantIndex) {
        auto rt = instanceKlass->getRuntimeConstantPool();
        auto field = rt->getField(constantIndex);
        if (field == nullptr) {
            PANIC("FieldID is null, constantIndex: %d", constantIndex);
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
            case ValueType::ARRAY:
                stack.pushReference(fieldValue);
                break;

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
}
