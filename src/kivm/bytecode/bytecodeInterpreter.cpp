//
// Created by kiva on 2018/3/21.
//

#include <kivm/bytecode/bytecodeInterpreter.h>
#include <kivm/bytecode/bytecodes.h>
#include <kivm/bytecode/execution.h>
#include <kivm/oop/instanceOop.h>
#include <kivm/oop/arrayOop.h>
#include <kivm/oop/primitiveOop.h>
#include <kivm/oop/mirrorOop.h>
#include <kivm/oop/method.h>
#include <climits>
#include <sparsepp/spp.h>
#include <deque>

#include "sharedInterpreter.h"

#define BEGIN(code, pc) \
    while ((code).validateOffset(pc)) \
        switch ((code)[(pc)++]) {
#define OTHERWISE() \
    default:
#define NEXT() break
#define END() }

#ifdef KIVM_INTERPRETER_DEBUG
#define OPCODE(opcode) \
    case OPC_##opcode: \
        D("interpreter: pc: %d, opcode: %d, name: %s", pc - 1, codeBlob[pc - 1], #opcode);
#else
#define OPCODE(opcode) \
    case OPC_##opcode:
#endif

namespace kivm {
    void ByteCodeInterpreter::initialize() {
        // do nothing
    }

    oop ByteCodeInterpreter::interp(JavaThread *thread) {
        Frame *currentFrame = thread->getCurrentFrame();
        auto currentMethod = currentFrame->getMethod();
        auto currentClass = currentMethod->getClass();
        const CodeBlob &codeBlob = currentMethod->getCodeBlob();
        u4 &pc = thread->_pc;

        D("currentMethod: %s.%s:%s",
            strings::toStdString(currentClass->getName()).c_str(),
            strings::toStdString(currentMethod->getName()).c_str(),
            strings::toStdString(currentMethod->getDescriptor()).c_str());

        Stack &stack = currentFrame->getStack();
        Locals &locals = currentFrame->getLocals();

        thread->enterSafepointIfNeeded();

#include "instruction.cpp-inc"

        return nullptr;
    }
}
