//
// Created by kiva on 2018/6/20.
//

#include <kivm/bytecode/invocationContext.h>
#include <kivm/runtime/javaThread.h>
#include <kivm/oop/instanceOop.h>
#include <kivm/oop/primitiveOop.h>
#include <kivm/oop/mirrorOop.h>
#include <kivm/oop/arrayOop.h>

namespace helper {
    using namespace kivm;

    void callMethod(MethodID *method, const std::list<oop> &args) {
        auto thread = Threads::currentThread();
        if (thread == nullptr) {
            // java.lang.IllegalStateException: not a Java thread
            return;
        }

        InvocationContext::invokeWithArgs(thread, method->_method, args);
        if (thread->isExceptionOccurred()) {
            KiVM::uncaughtException(thread);
        }
    }

    void convertMethods(InstanceKlass *instanceKlass) {
        for (auto e : instanceKlass->getDeclaredMethods()) {
            auto methodId = e.second;
            if (methodId->_method->isStatic()) {
                callMethod(methodId, {/* args */});
            }
        }
    }

    void convertStaticFields(InstanceKlass *instanceKlass) {
        for (auto e : instanceKlass->getStaticFields()) {
            auto fieldId = e.second;
            oop value = nullptr;
            if (!instanceKlass->getStaticFieldValue(fieldId, (oop *) &value)) {
                SHOULD_NOT_REACH_HERE();
            }

            switch (value->getMarkOop()->getOopType()) {
                case oopType::PRIMITIVE_OOP: {
                    auto field = fieldId->_field;
                    switch (field->getValueType()) {
                        case ValueType::INT:
                        case ValueType::CHAR:
                        case ValueType::BOOLEAN:
                        case ValueType::SHORT:
                        case ValueType::BYTE: {
                            auto realValue = (intOop) value;
                            jint val = realValue->getValue();
                            break;
                        }
                        case ValueType::FLOAT: {
                            auto realValue = (floatOop) value;
                            jfloat val = realValue->getValue();
                            break;
                        }
                        case ValueType::DOUBLE: {
                            auto realValue = (doubleOop) value;
                            jdouble val = realValue->getValue();
                        }
                        case ValueType::LONG: {
                            auto realValue = (longOop) value;
                            jlong val = realValue->getValue();
                        }
                        default:
                            SHOULD_NOT_REACH_HERE();
                    }
                    break;
                }
                case oopType::INSTANCE_OOP: {
                    auto realValue = (instanceOop) value;
                    break;
                }
                case oopType::TYPE_ARRAY_OOP:
                case oopType::OBJECT_ARRAY_OOP: {
                    auto realValue = (arrayOop) value;
                    for (int i = 0; i < realValue->getLength(); ++i) {
                        oop element = realValue->getElementAt(i);
                    }
                    break;
                }
            }
        }
    }
}
