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
    oop JavaCall::invokeDynamic(JavaThread *thread, Method *invokeExact,
                                         instanceOop MH, Stack *stack,
                                         const String &descriptor) {
        // TODO: obtain args from stack and call invokeExact with MH
        return nullptr;
    }
}
