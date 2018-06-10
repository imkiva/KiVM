//
// Created by kiva on 2018/4/14.
//
#include <kivm/bytecode/invocationContext.h>
#include <kivm/bytecode/execution.h>
#include <kivm/runtime/abstractThread.h>
#include <kivm/oop/primitiveOop.h>
#include <kivm/oop/mirrorOop.h>
#include <ffi.h>
#include <algorithm>

#define FILL_ARG_VALUE(value, field) \
        argsHolder[fillIndex].field = value; \
        argsPointer[fillIndex] = (void *) &argsHolder[fillIndex].field

#define FILL_ARG(popFunc, field) FILL_ARG_VALUE(_stack->popFunc(), field)

#define CALL_FACTORY(type, pushFunc, exp) \
        type r; \
        ffi_call(&cif, (void (*)()) nativeMethod, (void *) &r, argsPointer); \
        if (!_thread->isExceptionOccurred() && !stackIsAllocated) { \
            _stack->pushFunc(r); \
            resultOop = exp; \
        }

#define CALL(type, pushFunc, resultOopType) \
        CALL_FACTORY(type, pushFunc, new resultOopType(r))

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

    oop InvocationContext::invokeNative(bool hasThis, bool resolveTwice) {
        const std::vector<ValueType> &descriptorMap = _method->getArgumentValueTypesNoWrap();

        D("nativeInvocationContext: %s.%s:%s, static: %s, native: %s, nargs: %zd",
            strings::toStdString(_instanceKlass->getName()).c_str(),
            strings::toStdString(_method->getName()).c_str(),
            strings::toStdString(_method->getDescriptor()).c_str(),
            hasThis ? "false" : "true",
            _method->isNative() ? "true" : "false",
            descriptorMap.size());

        // native methods
        void *nativeMethod = _method->getNativePointer();
        if (nativeMethod == nullptr) {
            auto klass = (InstanceKlass *) BootstrapClassLoader::get()
                ->loadClass(L"java/lang/UnsatisfiedLinkError");
            const String &fixedName = strings::replaceAll(_instanceKlass->getName(),
                Global::SLASH, Global::DOT);
            _thread->throwException(klass, fixedName
                                           + L"."
                                           + _method->getName()
                                           + _method->getDescriptor());
            return nullptr;
        }

        bool stackIsAllocated = false;

        // TODO: make it elegant
        // XXX: Temporary workaround: allocate a stack to hold arguments
        if (_stack == nullptr) {
            D("nativeInvocation(call from args): allocate a stack to hold arguments");
            _stack = new Stack((int) _args.size());
            stackIsAllocated = true;
            int localVariableIndex = 0;
            bool isStatic = !hasThis;

            std::for_each(_args.begin(), _args.end(), [&](oop arg) {
                if (arg == nullptr) {
                    _stack->pushReference(nullptr);
                    return;
                }

                switch (arg->getMarkOop()->getOopType()) {
                    case oopType::INSTANCE_OOP:
                    case oopType::OBJECT_ARRAY_OOP:
                    case oopType::TYPE_ARRAY_OOP: {
                        D("nativeInvocationContext: copying reference: #%d - %p", localVariableIndex, arg);
                        localVariableIndex++;
                        _stack->pushReference(arg);
                        break;
                    }

                    case oopType::PRIMITIVE_OOP: {
                        ValueType valueType = descriptorMap[isStatic ? localVariableIndex : localVariableIndex - 1];
                        switch (valueType) {
                            case ValueType::INT: {
                                int value = ((intOop) arg)->getValue();
                                D("Copying int: #%d - %d", localVariableIndex, value);
                                localVariableIndex++;
                                _stack->pushInt(value);
                                break;
                            }
                            case ValueType::FLOAT: {
                                float value = ((floatOop) arg)->getValue();
                                D("Copying float: #%d - %f", localVariableIndex, value);
                                localVariableIndex++;
                                _stack->pushFloat(value);
                                break;
                            }
                            case ValueType::DOUBLE: {
                                double value = ((doubleOop) arg)->getValue();
                                D("Copying double: #%d - %lf", localVariableIndex, value);
                                localVariableIndex++;
                                _stack->pushDouble(value);
                                break;
                            }
                            case ValueType::LONG: {
                                long value = ((longOop) arg)->getValue();
                                D("Copying long: #%d - %ld", localVariableIndex, value);
                                localVariableIndex++;
                                _stack->pushLong(value);
                                break;
                            }
                            default:
                                PANIC("Unknown value type: %d", valueType);
                                break;
                        }
                        break;
                    }

                    default:
                        PANIC("Unknown oop type");
                }
            });
        }

        ValueType returnValueType = _method->getReturnTypeNoWrap();
        ffi_type *rtype = valueTypeToFFIType(returnValueType);

        // we need 2 more arguments
        // 1. JNIEnv *
        // 2. jobject or jclass
        auto argumentCount = ((int) descriptorMap.size()) + 2;

        // arguments to pass to ffi_call()
        void *argsPointer[argumentCount];
        ffi_type *argsType[argumentCount];

        // store actual value of arguments
        jvalue argsHolder[argumentCount];

        memset(argsHolder, '\0', sizeof(argsHolder));
        memset(argsPointer, '\0', sizeof(argsPointer));
        memset(argsType, '\0', sizeof(argsType));

        oop thisObject = nullptr;

        // fill arguments, `this` not included
        int fillIndex = argumentCount - 1;
        for (auto it = descriptorMap.rbegin(); it != descriptorMap.rend(); ++it, --fillIndex) {
            ValueType valueType = *it;

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
                    break;
                }
                default:
                    PANIC("Unknown value type: %d", valueType);
            }
        }

        if (fillIndex != 1) {
            PANIC("fillIndex should be 1");
        }

        if (!hasThis) {
            D("nativeInvocationContext: Pass jclass to static methods");
            FILL_ARG_VALUE((void *) _method->getClass(), l);
            argsType[fillIndex] = &ffi_type_pointer;

        } else {
            D("nativeInvocationContext: Pass jobject to member methods");
            FILL_ARG(popReference, l);
            argsType[fillIndex] = &ffi_type_pointer;

            thisObject = Resolver::javaOop(argsHolder[fillIndex].l);
            if (thisObject == nullptr) {
                _thread->throwException(Global::java_lang_NullPointerException);
                // TODO: make it elegant
                // XXX: Temporary workaround: allocate a stack to hold arguments
                if (stackIsAllocated) {
                    delete _stack;
                    _stack = nullptr;
                }
                return nullptr;
            }

            if (resolveTwice) {
                auto resolvedVirtualMethod = resolveVirtualMethod(thisObject, _method);
                if (resolvedVirtualMethod == nullptr) {
                    PANIC("resolveVirtualMethod: failed");
                }
                this->_method = resolvedVirtualMethod;
            }
        }

        if (--fillIndex != 0) {
            PANIC("fillIndex should be 0");
        }

        JavaVM *javaVM = KiVM::getJavaVMQuick();
        JNIEnv *env = nullptr;
        if (javaVM->GetEnv((void **) &env, JNI_VERSION_1_6) != JNI_OK) {
            PANIC("Failed to obtain JNIEnv");
        }

        D("nativeInvocationContext: Pass JNIEnv to native methods");
        FILL_ARG_VALUE((void *) env, l);
        argsType[fillIndex] = &ffi_type_pointer;

        // OK, let's make the call.
        // setup frame for native methods
        Frame frame(_method->getMaxLocals(), _method->getMaxStack());
        // something went wrong when preparing frame
        if (!prepareFrame(&frame)) {
            return nullptr;
        }

        // prepare the call
        ffi_cif cif{};
        ffi_status result = ffi_prep_cif(&cif, FFI_DEFAULT_ABI,
            (unsigned int) argumentCount,
            rtype, argsType);
        if (result != FFI_OK) {
            PANIC("invokeNative: ffi_prep_cif() failed: %d", result);
        }

        oop resultOop = nullptr;
        prepareSynchronized(thisObject);

        switch (returnValueType) {
            case ValueType::VOID: {
                ffi_call(&cif, (void (*)()) nativeMethod, nullptr, argsPointer);
                break;
            }
            case ValueType::BOOLEAN: {
                CALL(jint, pushInt, intOopDesc);
                break;
            }
            case ValueType::BYTE: {
                CALL(jint, pushInt, intOopDesc);
                break;
            }
            case ValueType::CHAR: {
                CALL(jint, pushInt, intOopDesc);
                break;
            }
            case ValueType::SHORT: {
                CALL(jint, pushInt, intOopDesc);
                break;
            }
            case ValueType::INT: {
                CALL(jint, pushInt, intOopDesc);
                break;
            }
            case ValueType::FLOAT: {
                CALL(jfloat, pushFloat, floatOopDesc);
                break;
            }
            case ValueType::LONG: {
                CALL(jlong, pushLong, longOopDesc);
                break;
            }
            case ValueType::DOUBLE: {
                CALL(jdouble, pushDouble, doubleOopDesc);
                break;
            }
            case ValueType::OBJECT:
            case ValueType::ARRAY: {
                CALL_FACTORY(jobject, pushReference, Resolver::javaOop(r));
                break;
            }

            default:
                PANIC("Unknown value type: %d", returnValueType);
        }

        finishSynchronized(thisObject);

        // TODO: make it elegant
        // XXX: Temporary workaround: allocate a stack to hold arguments
        if (stackIsAllocated) {
            delete _stack;
            _stack = nullptr;
        }

        // drop frame
        _thread->_frames.pop();
        _thread->_pc = frame.getReturnPc();

        return resultOop;
    }
}
