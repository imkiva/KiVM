//
// Created by kiva on 2018/4/14.
//
#include <kivm/bytecode/invocationContext.h>
#include <kivm/bytecode/execution.h>
#include <kivm/runtime/thread.h>
#include <kivm/oop/primitiveOop.h>
#include <kivm/oop/mirrorOop.h>
#include <ffi.h>

#define FILL_ARG_VALUE(value, field) \
        argsHolder[fillIndex].field = value; \
        argsPointer[fillIndex] = (void *) &argsHolder[fillIndex].field

#define FILL_ARG(popFunc, field) FILL_ARG_VALUE(_stack.popFunc(), field)

#define CALL(type, pushFunc) \
        type r; \
        ffi_call(&cif, (void (*)()) nativeMethod, (void *) &r, argsPointer); \
        _stack.pushFunc(r)

namespace kivm {
    static ffi_type *valueTypeToFFIType(ValueType valueType) {
        switch (valueType) {
            case ValueType::BOOLEAN:
                return &ffi_type_uint8;
            case ValueType::BYTE:
                return &ffi_type_sint8;
            case ValueType::CHAR:
                return &ffi_type_uint16;
            case ValueType::SHORT:
                return &ffi_type_sint16;
            case ValueType::INT:
                return &ffi_type_uint32;
            case ValueType::FLOAT:
                return &ffi_type_float;
            case ValueType::LONG:
                return &ffi_type_sint64;
            case ValueType::DOUBLE:
                return &ffi_type_double;
            case ValueType::VOID:
                return &ffi_type_void;
            case ValueType::OBJECT:
            case ValueType::ARRAY:
                return &ffi_type_pointer;

            default:
                PANIC("Unknown value type: %d", valueType);
        }
    }

    void InvocationContext::invokeNative(bool hasThis) {
        const std::vector<ValueType> &descriptorMap = _method->getArgumentValueTypesNoWrap();

        D("invokeTarget: %s.%s:%s, hasThis: %s, native: %s, nargs: %zd",
          strings::toStdString(_instanceKlass->getName()).c_str(),
          strings::toStdString(_method->getName()).c_str(),
          strings::toStdString(_method->getDescriptor()).c_str(),
          hasThis ? "true" : "false",
          _method->isNative() ? "true" : "false",
          descriptorMap.size());

        // native methods
        void *nativeMethod = _method->getNativePointer();
        if (nativeMethod == nullptr) {
            PANIC("UnsatisfiedLinkError: %s.%s:%s",
                  strings::toStdString(_instanceKlass->getName()).c_str(),
                  strings::toStdString(_method->getName()).c_str(),
                  strings::toStdString(_method->getDescriptor()).c_str());
        }

        ValueType returnValueType = _method->getReturnTypeNoWrap();
        ffi_type *rtype = valueTypeToFFIType(returnValueType);

        // we need 2 more arguments
        // 1. JNIEnv *
        // 2. jobject or jclass
        auto argumentCount = ((int) descriptorMap.size()) + 2;
        D("Calculated exact argument count: %d", argumentCount);

        // arguments to pass to ffi_call()
        void *argsPointer[argumentCount];
        ffi_type *argsType[argumentCount];

        // store actual value of arguments
        jvalue argsHolder[argumentCount];

        oop thisObject = nullptr;

        // fill arguments
        int fillIndex = argumentCount - 1;
        for (auto it = descriptorMap.rbegin(); it != descriptorMap.rend(); ++it, --fillIndex) {
            ValueType valueType = *it;
            D("Passing stack argument whose value type is %d", valueType);

            // fill types
            argsType[fillIndex] = valueTypeToFFIType(valueType);

            // fill values
            switch (valueType) {
                case ValueType::INT: {
                    FILL_ARG(popInt, i);
                    break;
                }
                case ValueType::SHORT: {
                    FILL_ARG(popInt, s);
                    break;
                }
                case ValueType::CHAR: {
                    FILL_ARG(popInt, c);
                    break;
                }
                case ValueType::BYTE: {
                    FILL_ARG(popInt, b);
                    break;
                }
                case ValueType::BOOLEAN: {
                    FILL_ARG(popInt, z);
                    break;
                }
                case ValueType::LONG: {
                    FILL_ARG(popLong, j);
                    break;
                }
                case ValueType::FLOAT: {
                    FILL_ARG(popFloat, f);
                    break;
                }
                case ValueType::DOUBLE: {
                    FILL_ARG(popDouble, d);
                    break;
                }
                case ValueType::OBJECT:
                case ValueType::ARRAY: {
                    FILL_ARG(popReference, l);
                    if (fillIndex == 0 && hasThis) {
                        thisObject = Resolver::resolveJObject(argsHolder[fillIndex].l);
                        if (thisObject == nullptr) {
                            PANIC("NullPointerException");
                        }
                    }
                    break;
                }
                default:
                    PANIC("Unknown value type: %d", valueType);
            }
        }

        D("Start to fill JNI interfaces, fillIndex: %d", fillIndex);
        // this is a static method, we should pass jclass to it
        if (!hasThis) {
            D("Pass jclass to static methods");
            FILL_ARG_VALUE((void *) _method->getClass(), l);
            argsType[fillIndex] = &ffi_type_pointer;
            --fillIndex;
        }

        if (fillIndex != 0) {
            PANIC("fillIndex should be 0");
        }

        JavaVM *javaVM = KiVM::getJavaVMQuick();
        JNIEnv *env = nullptr;
        if (javaVM->GetEnv((void **) &env, JNI_VERSION_1_6) != JNI_OK) {
            PANIC("Failed to obtain JNIEnv");
        }

        D("Pass JNIEnv to native methods");
        FILL_ARG_VALUE((void *) env, l);
        argsType[fillIndex] = &ffi_type_pointer;


        // OK, let's make the call.
        prepareSynchronized(thisObject);

        // prepare the call
        ffi_cif cif{};
        ffi_status result = ffi_prep_cif(&cif, FFI_DEFAULT_ABI,
                                         (unsigned int) argumentCount,
                                         rtype, argsType);
        if (result != FFI_OK) {
            PANIC("invokeNative: ffi_prep_cif() failed: %d", result);
        }

        // invoke and push the result onto the stack(if has)
        switch (returnValueType) {
            case ValueType::VOID: {
                ffi_call(&cif, (void (*)()) nativeMethod, nullptr, argsPointer);
                break;
            }
            case ValueType::BOOLEAN: {
                CALL(jboolean, pushInt);
                break;
            }
            case ValueType::BYTE: {
                CALL(jbyte, pushInt);
                break;
            }
            case ValueType::CHAR: {
                CALL(jchar, pushInt);
                break;
            }
            case ValueType::SHORT: {
                CALL(jshort, pushInt);
                break;
            }
            case ValueType::INT: {
                CALL(jint, pushInt);
                break;
            }
            case ValueType::FLOAT: {
                CALL(jfloat, pushFloat);
                break;
            }
            case ValueType::LONG: {
                CALL(jlong, pushLong);
                break;
            }
            case ValueType::DOUBLE: {
                CALL(jdouble, pushDouble);
                break;
            }
            case ValueType::OBJECT:
            case ValueType::ARRAY: {
                CALL(jobject, pushReference);
                break;
            }

            default:
                PANIC("Unknown value type: %d", returnValueType);
        }

        finishSynchronized(thisObject);
    }
}
