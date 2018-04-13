//
// Created by kiva on 2018/4/13.
//
#include <kivm/bytecode/invocationContext.h>
#include <kivm/bytecode/execution.h>
#include <kivm/runtime/thread.h>
#include <kivm/oop/primitiveOop.h>
#include <kivm/oop/mirrorOop.h>

namespace kivm {
    InvocationContext::InvocationContext(JavaThread *thread, Method *method, Stack &stack)
        : _thread(thread), _method(method), _stack(stack), _instanceKlass(_method->getClass()) {
    }

    void InvocationContext::prepareEnvironment() {
        Execution::initializeClass(_thread, _instanceKlass);
    }

    void InvocationContext::invoke(bool hasThis) {
        prepareEnvironment();

        // TODO: implement native method invocation
        if (_method->isNative()) {
            PANIC("Cannot invoke native method currently");
        }

        std::list<ValueType> descriptorMap = _method->getArgumentValueTypes();
        descriptorMap.reverse();

        D("invokeTarget: %s.%s:%s, hasThis: %s, nargs: %zd",
          strings::toStdString(_instanceKlass->getName()).c_str(),
          strings::toStdString(_method->getName()).c_str(),
          strings::toStdString(_method->getDescriptor()).c_str(),
          hasThis ? "true" : "false",
          descriptorMap.size());

        std::list<oop> callingArgs;
        std::for_each(descriptorMap.begin(), descriptorMap.end(), [&](ValueType valueType) {
            switch (valueType) {
                case ValueType::INT:
                    callingArgs.push_front(new intOopDesc(_stack.popInt()));
                    break;
                case ValueType::LONG:
                    callingArgs.push_front(new longOopDesc(_stack.popLong()));
                    break;
                case ValueType::FLOAT:
                    callingArgs.push_front(new floatOopDesc(_stack.popFloat()));
                    break;
                case ValueType::DOUBLE:
                    callingArgs.push_front(new doubleOopDesc(_stack.popDouble()));
                    break;
                case ValueType::OBJECT:
                    callingArgs.push_front(Resolver::resolveJObject(_stack.popReference()));
                    break;
                default:
                    PANIC("Unknown value type");
            }
        });

        oop thisObj = nullptr;
        if (hasThis) {
            thisObj = Resolver::resolveJObject(_stack.popReference());
            if (thisObj == nullptr) {
                PANIC("java.lang.NullPointerException");
            }
            callingArgs.push_front(thisObj);
        }

        prepareSynchronized(thisObj);
        oop result = _thread->runMethod(_method, callingArgs);
        switch (_method->getReturnType()) {
            case ValueType::INT:
                _stack.pushInt(((intOop) result)->getValue());
                break;
            case ValueType::LONG:
                _stack.pushLong(((longOop) result)->getValue());
                break;
            case ValueType::FLOAT:
                _stack.pushFloat(((floatOop) result)->getValue());
                break;
            case ValueType::DOUBLE:
                _stack.pushDouble(((doubleOop) result)->getValue());
                break;
            case ValueType::OBJECT:
                _stack.pushReference(result);
                break;
            case ValueType::VOID:
                break;

            default:
                PANIC("Unknown value type");
        }
        finishSynchronized(thisObj);
    }

    void InvocationContext::prepareSynchronized(oop thisObject) {
        if (_method->isSynchronized()) {
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
}
