//
// Created by kiva on 2018/4/19.
//
#include <kivm/bytecode/invocationContext.h>
#include <kivm/bytecode/execution.h>
#include <kivm/runtime/thread.h>
#include <kivm/oop/primitiveOop.h>
#include <kivm/oop/mirrorOop.h>

namespace kivm {
    oop InvocationContext::invokeJava(bool hasThis, bool resolveTwice) {
        const std::vector<ValueType> &descriptorMap = _method->getArgumentValueTypes();

        D("javaInvocationContext: invokeTarget: %s.%s:%s, static: %s, native: %s, nargs: %zd",
            strings::toStdString(_instanceKlass->getName()).c_str(),
            strings::toStdString(_method->getName()).c_str(),
            strings::toStdString(_method->getDescriptor()).c_str(),
            hasThis ? "false" : "true",
            _method->isNative() ? "true" : "false",
            descriptorMap.size());

        if (_obtainArgsFromStack && _stack != nullptr) {
            std::list<oop> callingArgs;
            for (auto it = descriptorMap.rbegin(); it != descriptorMap.rend(); ++it) {
                ValueType valueType = *it;
                D("javaInvocationContext: Passing stack argument whose value type is %d", valueType);

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
                        PANIC("Unknown value type: %d", valueType);
                }
            }

            if (hasThis) {
                oop thisObjectOnStack = Resolver::javaOop(_stack->popReference());
                callingArgs.push_front(thisObjectOnStack);
            }

            this->_args.swap(callingArgs);
        }

        oop thisObject = nullptr;
        if (hasThis) {
            thisObject = *_args.begin();
            if (thisObject == nullptr) {
                PANIC("java.lang.NullPointerException");
            }
        }

        bool x = _method->getName() == L"charsetForName";

        if (resolveTwice && thisObject != nullptr) {
            auto resolvedVirtualMethod = resolveVirtualMethod(thisObject, _method);
            if (resolvedVirtualMethod == nullptr) {
                PANIC("resolveVirtualMethod: failed");
            }
            this->_method = resolvedVirtualMethod;
        }

        D("javaInvocationContext: invoke and push result onto the stack (if has)");
        prepareSynchronized(thisObject);

        oop result = callInterpreter();

        if (_stack != nullptr && !_thread->isExceptionOccurred()) {
            switch (_method->getReturnType()) {
                case ValueType::INT:
                    _stack->pushInt(((intOop) result)->getValue());
                    break;
                case ValueType::LONG:
                    _stack->pushLong(((longOop) result)->getValue());
                    break;
                case ValueType::FLOAT:
                    _stack->pushFloat(((floatOop) result)->getValue());
                    break;
                case ValueType::DOUBLE:
                    _stack->pushDouble(((doubleOop) result)->getValue());
                    break;
                case ValueType::OBJECT:
                case ValueType::ARRAY:
                    _stack->pushReference(result);
                    break;
                case ValueType::VOID:
                    break;

                default:
                    PANIC("Unknown value type");
            }
        }
        finishSynchronized(thisObject);
        return result;
    }
}
