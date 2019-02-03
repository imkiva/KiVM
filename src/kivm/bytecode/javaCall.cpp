//
// Created by kiva on 2018/4/13.
//
#include <kivm/bytecode/javaCall.h>
#include <kivm/bytecode/execution.h>
#include <kivm/runtime/abstractThread.h>
#include <kivm/oop/oopfwd.h>
#include <kivm/oop/instanceOop.h>
#include <kivm/oop/primitiveOop.h>
#include <kivm/oop/mirrorOop.h>
#include <kivm/bytecode/interpreter.h>
#include <kivm/native/classNames.h>
#include <algorithm>
#include <kivm/runtime/runtimeConfig.h>
#include <kivm/bytecode/bytecodeInterpreter.h>

namespace kivm {
    JavaCall::JavaCall(JavaThread *thread, Method *method, Stack *stack)
        : _thread(thread), _method(method), _stack(stack), _instanceKlass(_method->getClass()),
          _obtainArgsFromStack(true) {
    }

    JavaCall::JavaCall(JavaThread *thread, Method *method, const std::list<oop> &args)
        : _thread(thread), _method(method), _stack(nullptr), _instanceKlass(_method->getClass()),
          _obtainArgsFromStack(false), _args(args) {
    }

    bool JavaCall::prepareEnvironment() {
        return Execution::initializeClass(_thread, _instanceKlass);
    }

    void JavaCall::prepareSynchronized(oop thisObject) {
        if (_method->isSynchronized()) {
            D("invocationContext: method is synchronized");
            if (_method->isStatic()) {
                _method->getClass()->getJavaMirror()->getMarkOop()->monitorEnter();
            } else {
                thisObject->getMarkOop()->monitorEnter();
            }
        }
    }

    void JavaCall::finishSynchronized(oop thisObject) {
        if (_method->isSynchronized()) {
            if (_method->isStatic()) {
                _method->getClass()->getJavaMirror()->getMarkOop()->monitorExit();
            } else {
                thisObject->getMarkOop()->monitorExit();
            }
        }
    }

    oop JavaCall::callInterpreter() {
        Frame frame(_method->getMaxLocals(), _method->getMaxStack());

        // something went wrong when preparing frame
        if (!prepareFrame(&frame)) {
            return nullptr;
        }

        Locals &locals = frame.getLocals();

        // copy args to local variable table
        int localVariableIndex = 0;
        int localSlotIndex = 0;
        bool isStatic = _method->isStatic();
        const std::vector<ValueType> descriptorMap = _method->getArgumentValueTypes();

        std::for_each(_args.begin(), _args.end(), [&](oop arg) {
            if (arg == nullptr) {
                locals.setReference(localSlotIndex++, nullptr);

            } else {
                switch (arg->getMarkOop()->getOopType()) {
                    case oopType::INSTANCE_OOP:
                    case oopType::OBJECT_ARRAY_OOP:
                    case oopType::TYPE_ARRAY_OOP: {
                        locals.setReference(localSlotIndex++, arg);
                        break;
                    }

                    case oopType::PRIMITIVE_OOP: {
                        ValueType valueType = descriptorMap[isStatic
                                                            ? localVariableIndex
                                                            : localVariableIndex - 1];
                        switch (valueType) {
                            case ValueType::INT: {
                                int value = ((intOop) arg)->getValue();
                                locals.setInt(localSlotIndex++, value);
                                break;
                            }
                            case ValueType::FLOAT: {
                                float value = ((floatOop) arg)->getValue();
                                locals.setFloat(localSlotIndex++, value);
                                break;
                            }
                            case ValueType::DOUBLE: {
                                double value = ((doubleOop) arg)->getValue();
                                locals.setDouble(localSlotIndex, value);
                                localSlotIndex += 2;
                                break;
                            }
                            case ValueType::LONG: {
                                jlong value = ((longOop) arg)->getValue();
                                locals.setLong(localSlotIndex, value);
                                localSlotIndex += 2;
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
            }
            ++localVariableIndex;
        });

        _thread->_pc = 0;
        oop result = DefaultInterpreter::interp(_thread);
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
            D("returned from %s.%s:%s to the Java Virtual Machine",
                strings::toStdString(_method->getClass()->getName()).c_str(),
                strings::toStdString(_method->getName()).c_str(),
                strings::toStdString(_method->getDescriptor()).c_str());

            if (_thread->isExceptionOccurred()) {
                KiVM::uncaughtException(_thread);
            }
        }
        return result;
    }

    bool JavaCall::prepareFrame(Frame *frame) {
        if (_thread->_frames.getSize() >= _thread->_frames.getMaxFrames()) {
            _thread->throwException((InstanceKlass *) BootstrapClassLoader::get()
                ->loadClass(L"java/lang/StackOverflowException"), false);
            return false;
        }

        frame->setMethod(_method);
        frame->setReturnPc(_thread->_pc);
        frame->setNativeFrame(_method->isNative());

        _thread->_frames.push(frame);
        return true;
    }

    bool JavaCall::fillArguments(const std::vector<ValueType> &argTypes, bool hasThis) {
        std::list<oop> callingArgs;
        for (auto it = argTypes.rbegin(); it != argTypes.rend(); ++it) {
            ValueType valueType = *it;

            switch (valueType) {
                case ValueType::INT:
                    callingArgs.push_front(new intOopDesc(_stack->popInt()));
                    break;
                case ValueType::LONG:
                    callingArgs.push_front(new longOopDesc(_stack->popLong()));
                    break;
                case ValueType::FLOAT:
                    callingArgs.push_front(new floatOopDesc(_stack->popFloat()));
                    break;
                case ValueType::DOUBLE:
                    callingArgs.push_front(new doubleOopDesc(_stack->popDouble()));
                    break;
                case ValueType::OBJECT:
                case ValueType::ARRAY:
                    callingArgs.push_front(Resolver::javaOop(_stack->popReference()));
                    break;
                default:
                    return false;
            }
        }

        if (hasThis) {
            oop thiz = Resolver::javaOop(_stack->popReference());
            callingArgs.push_front(thiz);
        }

        this->_args.swap(callingArgs);
        return true;
    }
}
