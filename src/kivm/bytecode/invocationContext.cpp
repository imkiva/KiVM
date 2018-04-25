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

    void InvocationContext::invoke(bool hasThis, bool resolveTwice) {
        prepareEnvironment();

        if (_method->isNative()) {
            this->invokeNative(hasThis, resolveTwice);

        } else {
            this->invokeJava(hasThis, resolveTwice);
        }
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
