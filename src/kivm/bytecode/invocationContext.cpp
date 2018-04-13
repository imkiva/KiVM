//
// Created by kiva on 2018/4/13.
//
#include <kivm/bytecode/invocationContext.h>
#include <kivm/bytecode/execution.h>

namespace kivm {
    InvocationContext::InvocationContext(JavaThread *thread, Method *method, Stack &stack)
        : _thread(thread), _method(method), _stack(stack), _instanceKlass(_method->getClass()), _nargs(0) {
    }

    void InvocationContext::prepareInvocation() {
        Execution::initializeClass(_thread, _instanceKlass);
        _nargs = _method->getArgumentCount();
    }

    void InvocationContext::invoke(bool hasThis) {
        prepareInvocation();
        D("invokeTarget: %s.%s:%s, hasThis: %s, nargs: %d",
          strings::toStdString(_instanceKlass->getName()).c_str(),
          strings::toStdString(_method->getName()).c_str(),
          strings::toStdString(_method->getDescriptor()).c_str(),
          hasThis ? "true" : "false",
          _nargs);

        PANIC("InvocationContext::invoke()");
    }
}
