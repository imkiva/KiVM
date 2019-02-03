//
// Created by kiva on 2019-01-31.
//

#include <kivm/bytecode/javaCall.h>
#include <kivm/bytecode/execution.h>
#include <kivm/bytecode/interpreter.h>
#include <kivm/bytecode/bytecodeInterpreter.h>
#include <kivm/runtime/abstractThread.h>
#include <kivm/runtime/runtimeConfig.h>
#include <kivm/native/classNames.h>
#include <kivm/oop/oopfwd.h>
#include <kivm/oop/instanceOop.h>
#include <kivm/oop/primitiveOop.h>
#include <kivm/oop/mirrorOop.h>
#include <algorithm>

namespace kivm {
    oop JavaCall::invokeDynamic(instanceOop MH, const String &descriptor) {
        // TODO: obtain args from stack and call invokeExact with MH
        _obtainArgsFromStack = true;
        if (_stack == nullptr) {
            SHOULD_NOT_REACH_HERE_M("Stack must not be null");
        }

        const std::vector<ValueType> &descriptorMap = parseArgumentValueTypes(descriptor);
        ValueType returnType = parseReturnValueType(descriptor);

        // Do not obtain this form stack
        // The `this` object is `MH`
        if (!fillArguments(descriptorMap, false)) {
            // TODO: throw StackOverflow
            PANIC("StackOverflow");
        }

        // The `this`
        _args.push_front(MH);

        assert(_method->isNative());
        return nullptr;
    }
}
