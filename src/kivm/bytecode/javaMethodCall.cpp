//
// Created by kiva on 2018/4/19.
//
#include <kivm/bytecode/javaCall.h>
#include <kivm/bytecode/execution.h>
#include <kivm/runtime/abstractThread.h>
#include <kivm/oop/primitiveOop.h>
#include <kivm/oop/mirrorOop.h>

namespace kivm {
    oop JavaCall::invokeJava(bool hasThis, bool resolveTwice) {
        const std::vector<ValueType> &descriptorMap = _method->getArgumentValueTypes();

        D("javaInvocationContext: %s.%s:%s, static: %s, native: %s, nargs: %zd",
            strings::toStdString(_instanceKlass->getName()).c_str(),
            strings::toStdString(_method->getName()).c_str(),
            strings::toStdString(_method->getDescriptor()).c_str(),
            hasThis ? "false" : "true",
            _method->isNative() ? "true" : "false",
            descriptorMap.size());

        if (_obtainArgsFromStack && _stack != nullptr) {
            if (!fillArguments(descriptorMap, hasThis)) {
                SHOULD_NOT_REACH_HERE_M("Unknown value type");
            }
        }

        oop thisObject = nullptr;
        if (hasThis) {
            thisObject = *_args.begin();
            if (thisObject == nullptr) {
                _thread->throwException(Global::java_lang_NullPointerException);
                return nullptr;
            }
        }

        if (resolveTwice && thisObject != nullptr) {
            auto resolvedVirtualMethod = resolveVirtualMethod(thisObject, _method);
            if (resolvedVirtualMethod == nullptr) {
                PANIC("resolveVirtualMethod: failed");
            }
            this->_method = resolvedVirtualMethod;
        }

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
