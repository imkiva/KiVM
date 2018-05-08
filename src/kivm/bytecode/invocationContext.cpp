//
// Created by kiva on 2018/4/13.
//
#include <kivm/bytecode/invocationContext.h>
#include <kivm/bytecode/execution.h>
#include <kivm/runtime/thread.h>
#include <kivm/oop/oopfwd.h>
#include <kivm/oop/instanceOop.h>
#include <kivm/oop/primitiveOop.h>
#include <kivm/oop/mirrorOop.h>
#include <kivm/bytecode/interpreter.h>
#include <kivm/native/classNames.h>
#include <algorithm>

namespace kivm {
    InvocationContext::InvocationContext(JavaThread *thread, Method *method, Stack *stack)
        : _thread(thread), _method(method), _stack(stack), _instanceKlass(_method->getClass()),
          _obtainArgsFromStack(true) {
    }

    InvocationContext::InvocationContext(JavaThread *thread, Method *method, const std::list<oop> &args)
        : _thread(thread), _method(method), _stack(nullptr), _instanceKlass(_method->getClass()),
          _obtainArgsFromStack(false), _args(args) {
    }

    void InvocationContext::prepareEnvironment() {
        Execution::initializeClass(_thread, _instanceKlass);
    }

    void InvocationContext::prepareSynchronized(oop thisObject) {
        if (_method->isSynchronized()) {
            D("invocationContext: method is synchronized");
            if (_method->isStatic()) {
                _method->getClass()->getJavaMirror()->getMarkOop()->monitorEnter();
            } else {
                thisObject->getMarkOop()->monitorEnter();
            }
        }
    }

    void InvocationContext::finishSynchronized(oop thisObject) {
        if (_method->isSynchronized()) {
            if (_method->isStatic()) {
                _method->getClass()->getJavaMirror()->getMarkOop()->monitorExit();
            } else {
                thisObject->getMarkOop()->monitorExit();
            }
        }
    }

    oop InvocationContext::callInterpreter() {
        D("### InvocationContext::callInterpreter(), maxLocals: %d, maxStack: %d",
            _method->getMaxLocals(), _method->getMaxStack());

        Frame frame(_method->getMaxLocals(), _method->getMaxStack());
        Locals &locals = frame.getLocals();

        // copy args to local variable table
        int localVariableIndex = 0;
        bool isStatic = _method->isStatic();
        const std::vector<ValueType> descriptorMap = _method->getArgumentValueTypes();

        std::for_each(_args.begin(), _args.end(), [&](oop arg) {
            if (arg == nullptr) {
                D("Copying reference: #%d - null", localVariableIndex);
                locals.setReference(localVariableIndex++, nullptr);
                return;
            }

            switch (arg->getMarkOop()->getOopType()) {
                case oopType::INSTANCE_OOP:
                case oopType::OBJECT_ARRAY_OOP:
                case oopType::TYPE_ARRAY_OOP: {
                    D("Copying reference: #%d - %p", localVariableIndex, arg);
                    locals.setReference(localVariableIndex++, arg);
                    break;
                }

                case oopType::PRIMITIVE_OOP: {
                    ValueType valueType = descriptorMap[isStatic ? localVariableIndex : localVariableIndex - 1];
                    switch (valueType) {
                        case ValueType::INT: {
                            int value = ((intOop) arg)->getValue();
                            D("Copying int: #%d - %d", localVariableIndex, value);
                            locals.setInt(localVariableIndex++, value);
                            break;
                        }
                        case ValueType::FLOAT: {
                            float value = ((floatOop) arg)->getValue();
                            D("Copying float: #%d - %f", localVariableIndex, value);
                            locals.setFloat(localVariableIndex++, value);
                            break;
                        }
                        case ValueType::DOUBLE: {
                            double value = ((doubleOop) arg)->getValue();
                            D("Copying double: #%d - %lf", localVariableIndex, value);
                            locals.setDouble(localVariableIndex++, value);
                            break;
                        }
                        case ValueType::LONG: {
                            long value = ((longOop) arg)->getValue();
                            D("Copying long: #%d - %ld", localVariableIndex, value);
                            locals.setLong(localVariableIndex++, value);
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

        // give them to interpreter
        frame.setMethod(_method);
        frame.setReturnPc(_thread->_pc);
        frame.setNativeFrame(_method->isNative());

        _thread->_frames.push(&frame);
        _thread->_pc = 0;
        oop result = ByteCodeInterpreter::interp(_thread);
        _thread->_frames.pop();
        _thread->_pc = frame.getReturnPc();

        if (_thread->_frames.getSize() > 0) {
            auto returnTo = _thread->_frames.getCurrentFrame()->getMethod();
            D("returned from %s.%s:%s to %s.%s:%s",
                strings::toStdString(_method->getClass()->getName()).c_str(),
                strings::toStdString(_method->getName()).c_str(),
                strings::toStdString(_method->getDescriptor()).c_str(),
                strings::toStdString(returnTo->getClass()->getName()).c_str(),
                strings::toStdString(returnTo->getName()).c_str(),
                strings::toStdString(returnTo->getDescriptor()).c_str());

        } else {
            D("returned from %s.%s:%s to the top method",
                strings::toStdString(_method->getClass()->getName()).c_str(),
                strings::toStdString(_method->getName()).c_str(),
                strings::toStdString(_method->getDescriptor()).c_str());

            if (_thread->isExceptionOccurred()) {
                auto ex = _thread->_exceptionOop;
                oop messageOop = nullptr;

                if (ex->getFieldValue(L"java/lang/Throwable", L"detailMessage", L"Ljava/lang/String;", &messageOop)) {

                    if (messageOop->getClass()->getClassType() == ClassType::INSTANCE_CLASS) {
                        auto instance = (instanceOop) messageOop;
                        PANIC("UncaughtException: %s: %s",
                            strings::toStdString(ex->getInstanceClass()->getName()).c_str(),
                            strings::toStdString(java::lang::String::toNativeString(instance)).c_str());
                    } else {
                        PANIC("UncaughtException: %s (failed to convert message oop)",
                            strings::toStdString(ex->getInstanceClass()->getName()).c_str());
                    }

                } else {
                    PANIC("UncaughtException: %s (failed to obtain message)",
                        strings::toStdString(ex->getInstanceClass()->getName()).c_str());
                }
            }
        }
        return result;
    }
}
