//
// Created by kiva on 2018/3/21.
//

#include <kivm/bytecode/threadedInterpreter.h>

#if defined(KIVM_THREADED) && !defined(KIVM_DEBUG)

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

#define BEGIN(...)
#define OTHERWISE() label_unrecoginized:
#define NEXT() goto *jump[codeBlob[pc++]];
#define END()

#define OPCODE_LABEL(opcode) \
    label_OPC_##opcode

#ifdef KIVM_INTERPRETER_DEBUG
#define OPCODE(opcode) \
    label_OPC_##opcode: \
        D("interpreter: pc: %d, opcode: %d, name: %s", pc - 1, codeBlob[pc - 1], #opcode);
#else
#define OPCODE(opcode) \
    OPCODE_LABEL(opcode):
#endif

namespace kivm {
    void *ThreadedInterpreter::_jumpTable[OPC_NUM_OPCODES] = {nullptr};

    void ThreadedInterpreter::initialize() {
        static u4 emptyPc;
        static Stack emptyStack(0);
        static Locals emptyLocals(0);
        static CodeBlob emptyCode;

        threaded(nullptr, ThreadedInterpreter::_jumpTable,
            nullptr, nullptr, nullptr,
            emptyCode, emptyPc,
            emptyStack, emptyLocals);
    }

    oop ThreadedInterpreter::interp(JavaThread *thread) {
        Frame *currentFrame = thread->getCurrentFrame();
        auto currentMethod = currentFrame->getMethod();
        auto currentClass = currentMethod->getClass();
        const CodeBlob &codeBlob = currentMethod->getCodeBlob();
        u4 &pc = thread->_pc;
        Stack &stack = currentFrame->getStack();
        Locals &locals = currentFrame->getLocals();

        D("currentMethod: %s.%s:%s",
            strings::toStdString(currentClass->getName()).c_str(),
            strings::toStdString(currentMethod->getName()).c_str(),
            strings::toStdString(currentMethod->getDescriptor()).c_str());

        thread->enterSafepointIfNeeded();

        return threaded(thread, ThreadedInterpreter::_jumpTable,
            currentFrame, currentMethod, currentClass,
            codeBlob, pc, stack, locals);
    }

    oop ThreadedInterpreter::threaded(JavaThread *thread, void **jump,
                                      Frame *currentFrame,
                                      Method *currentMethod,
                                      InstanceKlass *currentClass,
                                      const CodeBlob &codeBlob,
                                      u4 &pc,
                                      Stack &stack,
                                      Locals &locals) {
        if (thread != nullptr && jump[0] != nullptr) {
            NEXT();
        }

        jump[OPC_NOP] = &&OPCODE_LABEL(NOP);
        jump[OPC_ACONST_NULL] = &&OPCODE_LABEL(ACONST_NULL);
        jump[OPC_ICONST_M1] = &&OPCODE_LABEL(ICONST_M1);
        jump[OPC_ICONST_0] = &&OPCODE_LABEL(ICONST_0);
        jump[OPC_ICONST_1] = &&OPCODE_LABEL(ICONST_1);
        jump[OPC_ICONST_2] = &&OPCODE_LABEL(ICONST_2);
        jump[OPC_ICONST_3] = &&OPCODE_LABEL(ICONST_3);
        jump[OPC_ICONST_4] = &&OPCODE_LABEL(ICONST_4);
        jump[OPC_ICONST_5] = &&OPCODE_LABEL(ICONST_5);
        jump[OPC_LCONST_0] = &&OPCODE_LABEL(LCONST_0);
        jump[OPC_LCONST_1] = &&OPCODE_LABEL(LCONST_1);
        jump[OPC_FCONST_0] = &&OPCODE_LABEL(FCONST_0);
        jump[OPC_FCONST_1] = &&OPCODE_LABEL(FCONST_1);
        jump[OPC_FCONST_2] = &&OPCODE_LABEL(FCONST_2);
        jump[OPC_DCONST_0] = &&OPCODE_LABEL(DCONST_0);
        jump[OPC_DCONST_1] = &&OPCODE_LABEL(DCONST_1);
        jump[OPC_BIPUSH] = &&OPCODE_LABEL(BIPUSH);
        jump[OPC_SIPUSH] = &&OPCODE_LABEL(SIPUSH);
        jump[OPC_LDC] = &&OPCODE_LABEL(LDC);
        jump[OPC_LDC_W] = &&OPCODE_LABEL(LDC_W);
        jump[OPC_LDC2_W] = &&OPCODE_LABEL(LDC2_W);
        jump[OPC_ILOAD] = &&OPCODE_LABEL(ILOAD);
        jump[OPC_LLOAD] = &&OPCODE_LABEL(LLOAD);
        jump[OPC_FLOAD] = &&OPCODE_LABEL(FLOAD);
        jump[OPC_DLOAD] = &&OPCODE_LABEL(DLOAD);
        jump[OPC_ALOAD] = &&OPCODE_LABEL(ALOAD);
        jump[OPC_ILOAD_0] = &&OPCODE_LABEL(ILOAD_0);
        jump[OPC_ILOAD_1] = &&OPCODE_LABEL(ILOAD_1);
        jump[OPC_ILOAD_2] = &&OPCODE_LABEL(ILOAD_2);
        jump[OPC_ILOAD_3] = &&OPCODE_LABEL(ILOAD_3);
        jump[OPC_LLOAD_0] = &&OPCODE_LABEL(LLOAD_0);
        jump[OPC_LLOAD_1] = &&OPCODE_LABEL(LLOAD_1);
        jump[OPC_LLOAD_2] = &&OPCODE_LABEL(LLOAD_2);
        jump[OPC_LLOAD_3] = &&OPCODE_LABEL(LLOAD_3);
        jump[OPC_FLOAD_0] = &&OPCODE_LABEL(FLOAD_0);
        jump[OPC_FLOAD_1] = &&OPCODE_LABEL(FLOAD_1);
        jump[OPC_FLOAD_2] = &&OPCODE_LABEL(FLOAD_2);
        jump[OPC_FLOAD_3] = &&OPCODE_LABEL(FLOAD_3);
        jump[OPC_DLOAD_0] = &&OPCODE_LABEL(DLOAD_0);
        jump[OPC_DLOAD_1] = &&OPCODE_LABEL(DLOAD_1);
        jump[OPC_DLOAD_2] = &&OPCODE_LABEL(DLOAD_2);
        jump[OPC_DLOAD_3] = &&OPCODE_LABEL(DLOAD_3);
        jump[OPC_ALOAD_0] = &&OPCODE_LABEL(ALOAD_0);
        jump[OPC_ALOAD_1] = &&OPCODE_LABEL(ALOAD_1);
        jump[OPC_ALOAD_2] = &&OPCODE_LABEL(ALOAD_2);
        jump[OPC_ALOAD_3] = &&OPCODE_LABEL(ALOAD_3);
        jump[OPC_IALOAD] = &&OPCODE_LABEL(IALOAD);
        jump[OPC_LALOAD] = &&OPCODE_LABEL(LALOAD);
        jump[OPC_FALOAD] = &&OPCODE_LABEL(FALOAD);
        jump[OPC_DALOAD] = &&OPCODE_LABEL(DALOAD);
        jump[OPC_AALOAD] = &&OPCODE_LABEL(AALOAD);
        jump[OPC_BALOAD] = &&OPCODE_LABEL(BALOAD);
        jump[OPC_CALOAD] = &&OPCODE_LABEL(CALOAD);
        jump[OPC_SALOAD] = &&OPCODE_LABEL(SALOAD);
        jump[OPC_ISTORE] = &&OPCODE_LABEL(ISTORE);
        jump[OPC_LSTORE] = &&OPCODE_LABEL(LSTORE);
        jump[OPC_FSTORE] = &&OPCODE_LABEL(FSTORE);
        jump[OPC_DSTORE] = &&OPCODE_LABEL(DSTORE);
        jump[OPC_ASTORE] = &&OPCODE_LABEL(ASTORE);
        jump[OPC_ISTORE_0] = &&OPCODE_LABEL(ISTORE_0);
        jump[OPC_ISTORE_1] = &&OPCODE_LABEL(ISTORE_1);
        jump[OPC_ISTORE_2] = &&OPCODE_LABEL(ISTORE_2);
        jump[OPC_ISTORE_3] = &&OPCODE_LABEL(ISTORE_3);
        jump[OPC_LSTORE_0] = &&OPCODE_LABEL(LSTORE_0);
        jump[OPC_LSTORE_1] = &&OPCODE_LABEL(LSTORE_1);
        jump[OPC_LSTORE_2] = &&OPCODE_LABEL(LSTORE_2);
        jump[OPC_LSTORE_3] = &&OPCODE_LABEL(LSTORE_3);
        jump[OPC_FSTORE_0] = &&OPCODE_LABEL(FSTORE_0);
        jump[OPC_FSTORE_1] = &&OPCODE_LABEL(FSTORE_1);
        jump[OPC_FSTORE_2] = &&OPCODE_LABEL(FSTORE_2);
        jump[OPC_FSTORE_3] = &&OPCODE_LABEL(FSTORE_3);
        jump[OPC_DSTORE_0] = &&OPCODE_LABEL(DSTORE_0);
        jump[OPC_DSTORE_1] = &&OPCODE_LABEL(DSTORE_1);
        jump[OPC_DSTORE_2] = &&OPCODE_LABEL(DSTORE_2);
        jump[OPC_DSTORE_3] = &&OPCODE_LABEL(DSTORE_3);
        jump[OPC_ASTORE_0] = &&OPCODE_LABEL(ASTORE_0);
        jump[OPC_ASTORE_1] = &&OPCODE_LABEL(ASTORE_1);
        jump[OPC_ASTORE_2] = &&OPCODE_LABEL(ASTORE_2);
        jump[OPC_ASTORE_3] = &&OPCODE_LABEL(ASTORE_3);
        jump[OPC_IASTORE] = &&OPCODE_LABEL(IASTORE);
        jump[OPC_LASTORE] = &&OPCODE_LABEL(LASTORE);
        jump[OPC_FASTORE] = &&OPCODE_LABEL(FASTORE);
        jump[OPC_DASTORE] = &&OPCODE_LABEL(DASTORE);
        jump[OPC_AASTORE] = &&OPCODE_LABEL(AASTORE);
        jump[OPC_BASTORE] = &&OPCODE_LABEL(BASTORE);
        jump[OPC_CASTORE] = &&OPCODE_LABEL(CASTORE);
        jump[OPC_SASTORE] = &&OPCODE_LABEL(SASTORE);
        jump[OPC_POP] = &&OPCODE_LABEL(POP);
        jump[OPC_POP2] = &&OPCODE_LABEL(POP2);
        jump[OPC_DUP] = &&OPCODE_LABEL(DUP);
        jump[OPC_DUP_X1] = &&OPCODE_LABEL(DUP_X1);
        jump[OPC_DUP_X2] = &&OPCODE_LABEL(DUP_X2);
        jump[OPC_DUP2] = &&OPCODE_LABEL(DUP2);
        jump[OPC_DUP2_X1] = &&OPCODE_LABEL(DUP2_X1);
        jump[OPC_DUP2_X2] = &&OPCODE_LABEL(DUP2_X2);
        jump[OPC_SWAP] = &&OPCODE_LABEL(SWAP);
        jump[OPC_IADD] = &&OPCODE_LABEL(IADD);
        jump[OPC_LADD] = &&OPCODE_LABEL(LADD);
        jump[OPC_FADD] = &&OPCODE_LABEL(FADD);
        jump[OPC_DADD] = &&OPCODE_LABEL(DADD);
        jump[OPC_ISUB] = &&OPCODE_LABEL(ISUB);
        jump[OPC_LSUB] = &&OPCODE_LABEL(LSUB);
        jump[OPC_FSUB] = &&OPCODE_LABEL(FSUB);
        jump[OPC_DSUB] = &&OPCODE_LABEL(DSUB);
        jump[OPC_IMUL] = &&OPCODE_LABEL(IMUL);
        jump[OPC_LMUL] = &&OPCODE_LABEL(LMUL);
        jump[OPC_FMUL] = &&OPCODE_LABEL(FMUL);
        jump[OPC_DMUL] = &&OPCODE_LABEL(DMUL);
        jump[OPC_IDIV] = &&OPCODE_LABEL(IDIV);
        jump[OPC_LDIV] = &&OPCODE_LABEL(LDIV);
        jump[OPC_FDIV] = &&OPCODE_LABEL(FDIV);
        jump[OPC_DDIV] = &&OPCODE_LABEL(DDIV);
        jump[OPC_IREM] = &&OPCODE_LABEL(IREM);
        jump[OPC_LREM] = &&OPCODE_LABEL(LREM);
        jump[OPC_FREM] = &&OPCODE_LABEL(FREM);
        jump[OPC_DREM] = &&OPCODE_LABEL(DREM);
        jump[OPC_INEG] = &&OPCODE_LABEL(INEG);
        jump[OPC_LNEG] = &&OPCODE_LABEL(LNEG);
        jump[OPC_FNEG] = &&OPCODE_LABEL(FNEG);
        jump[OPC_DNEG] = &&OPCODE_LABEL(DNEG);
        jump[OPC_ISHL] = &&OPCODE_LABEL(ISHL);
        jump[OPC_LSHL] = &&OPCODE_LABEL(LSHL);
        jump[OPC_ISHR] = &&OPCODE_LABEL(ISHR);
        jump[OPC_LSHR] = &&OPCODE_LABEL(LSHR);
        jump[OPC_IUSHR] = &&OPCODE_LABEL(IUSHR);
        jump[OPC_LUSHR] = &&OPCODE_LABEL(LUSHR);
        jump[OPC_IAND] = &&OPCODE_LABEL(IAND);
        jump[OPC_LAND] = &&OPCODE_LABEL(LAND);
        jump[OPC_IOR] = &&OPCODE_LABEL(IOR);
        jump[OPC_LOR] = &&OPCODE_LABEL(LOR);
        jump[OPC_IXOR] = &&OPCODE_LABEL(IXOR);
        jump[OPC_LXOR] = &&OPCODE_LABEL(LXOR);
        jump[OPC_IINC] = &&OPCODE_LABEL(IINC);
        jump[OPC_I2L] = &&OPCODE_LABEL(I2L);
        jump[OPC_I2F] = &&OPCODE_LABEL(I2F);
        jump[OPC_I2D] = &&OPCODE_LABEL(I2D);
        jump[OPC_L2I] = &&OPCODE_LABEL(L2I);
        jump[OPC_L2F] = &&OPCODE_LABEL(L2F);
        jump[OPC_L2D] = &&OPCODE_LABEL(L2D);
        jump[OPC_F2I] = &&OPCODE_LABEL(F2I);
        jump[OPC_F2L] = &&OPCODE_LABEL(F2L);
        jump[OPC_F2D] = &&OPCODE_LABEL(F2D);
        jump[OPC_D2I] = &&OPCODE_LABEL(D2I);
        jump[OPC_D2L] = &&OPCODE_LABEL(D2L);
        jump[OPC_D2F] = &&OPCODE_LABEL(D2F);
        jump[OPC_I2B] = &&OPCODE_LABEL(I2B);
        jump[OPC_I2C] = &&OPCODE_LABEL(I2C);
        jump[OPC_I2S] = &&OPCODE_LABEL(I2S);
        jump[OPC_LCMP] = &&OPCODE_LABEL(LCMP);
        jump[OPC_FCMPL] = &&OPCODE_LABEL(FCMPL);
        jump[OPC_FCMPG] = &&OPCODE_LABEL(FCMPG);
        jump[OPC_DCMPL] = &&OPCODE_LABEL(DCMPL);
        jump[OPC_DCMPG] = &&OPCODE_LABEL(DCMPG);
        jump[OPC_IFEQ] = &&OPCODE_LABEL(IFEQ);
        jump[OPC_IFNE] = &&OPCODE_LABEL(IFNE);
        jump[OPC_IFLT] = &&OPCODE_LABEL(IFLT);
        jump[OPC_IFGE] = &&OPCODE_LABEL(IFGE);
        jump[OPC_IFGT] = &&OPCODE_LABEL(IFGT);
        jump[OPC_IFLE] = &&OPCODE_LABEL(IFLE);
        jump[OPC_IF_ICMPEQ] = &&OPCODE_LABEL(IF_ICMPEQ);
        jump[OPC_IF_ICMPNE] = &&OPCODE_LABEL(IF_ICMPNE);
        jump[OPC_IF_ICMPLT] = &&OPCODE_LABEL(IF_ICMPLT);
        jump[OPC_IF_ICMPGE] = &&OPCODE_LABEL(IF_ICMPGE);
        jump[OPC_IF_ICMPGT] = &&OPCODE_LABEL(IF_ICMPGT);
        jump[OPC_IF_ICMPLE] = &&OPCODE_LABEL(IF_ICMPLE);
        jump[OPC_IF_ACMPEQ] = &&OPCODE_LABEL(IF_ACMPEQ);
        jump[OPC_IF_ACMPNE] = &&OPCODE_LABEL(IF_ACMPNE);
        jump[OPC_GOTO] = &&OPCODE_LABEL(GOTO);
        jump[OPC_JSR] = &&OPCODE_LABEL(JSR);
        jump[OPC_RET] = &&OPCODE_LABEL(RET);
        jump[OPC_TABLESWITCH] = &&OPCODE_LABEL(TABLESWITCH);
        jump[OPC_LOOKUPSWITCH] = &&OPCODE_LABEL(LOOKUPSWITCH);
        jump[OPC_IRETURN] = &&OPCODE_LABEL(IRETURN);
        jump[OPC_LRETURN] = &&OPCODE_LABEL(LRETURN);
        jump[OPC_FRETURN] = &&OPCODE_LABEL(FRETURN);
        jump[OPC_DRETURN] = &&OPCODE_LABEL(DRETURN);
        jump[OPC_ARETURN] = &&OPCODE_LABEL(ARETURN);
        jump[OPC_RETURN] = &&OPCODE_LABEL(RETURN);
        jump[OPC_GETSTATIC] = &&OPCODE_LABEL(GETSTATIC);
        jump[OPC_PUTSTATIC] = &&OPCODE_LABEL(PUTSTATIC);
        jump[OPC_GETFIELD] = &&OPCODE_LABEL(GETFIELD);
        jump[OPC_PUTFIELD] = &&OPCODE_LABEL(PUTFIELD);
        jump[OPC_INVOKEVIRTUAL] = &&OPCODE_LABEL(INVOKEVIRTUAL);
        jump[OPC_INVOKESPECIAL] = &&OPCODE_LABEL(INVOKESPECIAL);
        jump[OPC_INVOKESTATIC] = &&OPCODE_LABEL(INVOKESTATIC);
        jump[OPC_INVOKEINTERFACE] = &&OPCODE_LABEL(INVOKEINTERFACE);
        jump[OPC_INVOKEDYNAMIC] = &&OPCODE_LABEL(INVOKEDYNAMIC);
        jump[OPC_NEW] = &&OPCODE_LABEL(NEW);
        jump[OPC_NEWARRAY] = &&OPCODE_LABEL(NEWARRAY);
        jump[OPC_ANEWARRAY] = &&OPCODE_LABEL(ANEWARRAY);
        jump[OPC_ARRAYLENGTH] = &&OPCODE_LABEL(ARRAYLENGTH);
        jump[OPC_ATHROW] = &&OPCODE_LABEL(ATHROW);
        jump[OPC_CHECKCAST] = &&OPCODE_LABEL(CHECKCAST);
        jump[OPC_INSTANCEOF] = &&OPCODE_LABEL(INSTANCEOF);
        jump[OPC_MONITORENTER] = &&OPCODE_LABEL(MONITORENTER);
        jump[OPC_MONITOREXIT] = &&OPCODE_LABEL(MONITOREXIT);
        jump[OPC_WIDE] = &&OPCODE_LABEL(WIDE);
        jump[OPC_MULTIANEWARRAY] = &&OPCODE_LABEL(MULTIANEWARRAY);
        jump[OPC_IFNULL] = &&OPCODE_LABEL(IFNULL);
        jump[OPC_IFNONNULL] = &&OPCODE_LABEL(IFNONNULL);
        jump[OPC_GOTO_W] = &&OPCODE_LABEL(GOTO_W);
        jump[OPC_JSR_W] = &&OPCODE_LABEL(JSR_W);
        return nullptr;

#include "instruction.cpp-inc"

        return nullptr;
    }
}

#endif
