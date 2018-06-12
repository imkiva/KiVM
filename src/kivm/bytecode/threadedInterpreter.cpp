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

#define BEGIN()
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

        OPCODE(NOP)
        {
            NEXT();
        }
        OPCODE(ACONST_NULL)
        {
            stack.pushReference(nullptr);
            NEXT();
        }
        OPCODE(ICONST_M1)
        {
            stack.pushInt(-1);
            NEXT();
        }
        OPCODE(ICONST_0)
        {
            stack.pushInt(0);
            NEXT();
        }
        OPCODE(ICONST_1)
        {
            stack.pushInt(1);
            NEXT();
        }
        OPCODE(ICONST_2)
        {
            stack.pushInt(2);
            NEXT();
        }
        OPCODE(ICONST_3)
        {
            stack.pushInt(3);
            NEXT();
        }
        OPCODE(ICONST_4)
        {
            stack.pushInt(4);
            NEXT();
        }
        OPCODE(ICONST_5)
        {
            stack.pushInt(5);
            NEXT();
        }
        OPCODE(LCONST_0)
        {
            stack.pushLong(0);
            NEXT();
        }
        OPCODE(LCONST_1)
        {
            stack.pushLong(1);
            NEXT();
        }
        OPCODE(FCONST_0)
        {
            stack.pushFloat(0);
            NEXT();
        }
        OPCODE(FCONST_1)
        {
            stack.pushFloat(1);
            NEXT();
        }
        OPCODE(FCONST_2)
        {
            stack.pushFloat(2);
            NEXT();
        }
        OPCODE(DCONST_0)
        {
            stack.pushDouble(0);
            NEXT();
        }
        OPCODE(DCONST_1)
        {
            stack.pushDouble(1);
            NEXT();
        }
        OPCODE(BIPUSH)
        {
            auto bi = (signed char) codeBlob[pc++];
            stack.pushInt(bi);
            NEXT();
        }
        OPCODE(SIPUSH)
        {
            short si = codeBlob[pc] << 8 | codeBlob[pc + 1];
            pc += 2;
            stack.pushInt(si);
            NEXT();
        }
        OPCODE(LDC)
        {
            int constantIndex = codeBlob[pc++];
            Execution::loadConstant(currentClass->getRuntimeConstantPool(),
                stack, constantIndex);
            NEXT();
        }
        OPCODE(LDC_W)
        {
            int constantIndex = codeBlob[pc] << 8 | codeBlob[pc + 1];
            pc += 2;
            Execution::loadConstant(currentClass->getRuntimeConstantPool(),
                stack, constantIndex);
            NEXT();
        }
        OPCODE(LDC2_W)
        {
            int constantIndex = codeBlob[pc] << 8 | codeBlob[pc + 1];
            pc += 2;
            Execution::loadConstant(currentClass->getRuntimeConstantPool(),
                stack, constantIndex);
            NEXT();
        }
        OPCODE(ILOAD)
        {
            int index = codeBlob[pc++];
            stack.pushInt(locals.getInt(index));
            NEXT();
        }
        OPCODE(LLOAD)
        {
            int index = codeBlob[pc++];
            stack.pushLong(locals.getLong(index));
            NEXT();
        }
        OPCODE(FLOAD)
        {
            int index = codeBlob[pc++];
            stack.pushFloat(locals.getFloat(index));
            NEXT();
        }
        OPCODE(DLOAD)
        {
            int index = codeBlob[pc++];
            stack.pushDouble(locals.getDouble(index));
            NEXT();
        }
        OPCODE(ALOAD)
        {
            int index = codeBlob[pc++];
            stack.pushReference(locals.getReference(index));
            NEXT();
        }
        OPCODE(ILOAD_0)
        {
            stack.pushInt(locals.getInt(0));
            NEXT();
        }
        OPCODE(ILOAD_1)
        {
            stack.pushInt(locals.getInt(1));
            NEXT();
        }
        OPCODE(ILOAD_2)
        {
            stack.pushInt(locals.getInt(2));
            NEXT();
        }
        OPCODE(ILOAD_3)
        {
            stack.pushInt(locals.getInt(3));
            NEXT();
        }
        OPCODE(LLOAD_0)
        {
            stack.pushLong(locals.getLong(0));
            NEXT();
        }
        OPCODE(LLOAD_1)
        {
            stack.pushLong(locals.getLong(1));
            NEXT();
        }
        OPCODE(LLOAD_2)
        {
            stack.pushLong(locals.getLong(2));
            NEXT();
        }
        OPCODE(LLOAD_3)
        {
            stack.pushLong(locals.getLong(3));
            NEXT();
        }
        OPCODE(FLOAD_0)
        {
            stack.pushFloat(locals.getFloat(0));
            NEXT();
        }
        OPCODE(FLOAD_1)
        {
            stack.pushFloat(locals.getFloat(1));
            NEXT();
        }
        OPCODE(FLOAD_2)
        {
            stack.pushFloat(locals.getFloat(2));
            NEXT();
        }
        OPCODE(FLOAD_3)
        {
            stack.pushFloat(locals.getFloat(3));
            NEXT();
        }
        OPCODE(DLOAD_0)
        {
            stack.pushDouble(locals.getDouble(0));
            NEXT();
        }
        OPCODE(DLOAD_1)
        {
            stack.pushDouble(locals.getDouble(1));
            NEXT();
        }
        OPCODE(DLOAD_2)
        {
            stack.pushDouble(locals.getDouble(2));
            NEXT();
        }
        OPCODE(DLOAD_3)
        {
            stack.pushDouble(locals.getDouble(3));
            NEXT();
        }
        OPCODE(ALOAD_0)
        {
            stack.pushReference(locals.getReference(0));
            NEXT();
        }
        OPCODE(ALOAD_1)
        {
            stack.pushReference(locals.getReference(1));
            NEXT();
        }
        OPCODE(ALOAD_2)
        {
            stack.pushReference(locals.getReference(2));
            NEXT();
        }
        OPCODE(ALOAD_3)
        {
            stack.pushReference(locals.getReference(3));
            NEXT();
        }
        OPCODE(IALOAD)
        OPCODE(SALOAD)
        OPCODE(CALOAD)
        OPCODE(BALOAD)
        {
            LOAD_ARRAY_ELEMENT(intOop, element, typeArray, pushInt, element->getValue());
            NEXT();
        }

        OPCODE(LALOAD)
        {
            LOAD_ARRAY_ELEMENT(longOop, element, typeArray, pushLong, element->getValue());
            NEXT();
        }
        OPCODE(FALOAD)
        {
            LOAD_ARRAY_ELEMENT(floatOop, element, typeArray, pushFloat, element->getValue());
            NEXT();
        }
        OPCODE(DALOAD)
        {
            LOAD_ARRAY_ELEMENT(doubleOop, element, typeArray, pushDouble, element->getValue());
            NEXT();
        }
        OPCODE(AALOAD)
        {
            LOAD_ARRAY_ELEMENT(oop, element, objectArray, pushReference, element);
            NEXT();
        }
        OPCODE(ISTORE)
        {
            int localIndex = codeBlob[pc++];
            locals.setInt(localIndex, stack.popInt());
            NEXT();
        }
        OPCODE(LSTORE)
        {
            int localIndex = codeBlob[pc++];
            locals.setLong(localIndex, stack.popLong());
            NEXT();
        }
        OPCODE(FSTORE)
        {
            int localIndex = codeBlob[pc++];
            locals.setFloat(localIndex, stack.popFloat());
            NEXT();
        }
        OPCODE(DSTORE)
        {
            int localIndex = codeBlob[pc++];
            locals.setDouble(localIndex, stack.popDouble());
            NEXT();
        }
        OPCODE(ASTORE)
        {
            int localIndex = codeBlob[pc++];
            locals.setReference(localIndex, stack.popReference());
            NEXT();
        }
        OPCODE(ISTORE_0)
        {
            locals.setInt(0, stack.popInt());
            NEXT();
        }
        OPCODE(ISTORE_1)
        {
            locals.setInt(1, stack.popInt());
            NEXT();
        }
        OPCODE(ISTORE_2)
        {
            locals.setInt(2, stack.popInt());
            NEXT();
        }
        OPCODE(ISTORE_3)
        {
            locals.setInt(3, stack.popInt());
            NEXT();
        }
        OPCODE(LSTORE_0)
        {
            locals.setLong(0, stack.popLong());
            NEXT();
        }
        OPCODE(LSTORE_1)
        {
            locals.setLong(1, stack.popLong());
            NEXT();
        }
        OPCODE(LSTORE_2)
        {
            locals.setLong(2, stack.popLong());
            NEXT();
        }
        OPCODE(LSTORE_3)
        {
            locals.setLong(3, stack.popLong());
            NEXT();
        }
        OPCODE(FSTORE_0)
        {
            locals.setFloat(0, stack.popFloat());
            NEXT();
        }
        OPCODE(FSTORE_1)
        {
            locals.setFloat(1, stack.popFloat());
            NEXT();
        }
        OPCODE(FSTORE_2)
        {
            locals.setFloat(2, stack.popFloat());
            NEXT();
        }
        OPCODE(FSTORE_3)
        {
            locals.setFloat(3, stack.popFloat());
            NEXT();
        }
        OPCODE(DSTORE_0)
        {
            locals.setDouble(0, stack.popDouble());
            NEXT();
        }
        OPCODE(DSTORE_1)
        {
            locals.setDouble(1, stack.popDouble());
            NEXT();
        }
        OPCODE(DSTORE_2)
        {
            locals.setDouble(2, stack.popDouble());
            NEXT();
        }
        OPCODE(DSTORE_3)
        {
            locals.setDouble(3, stack.popDouble());
            NEXT();
        }
        OPCODE(ASTORE_0)
        {
            locals.setReference(0, stack.popReference());
            NEXT();
        }
        OPCODE(ASTORE_1)
        {
            locals.setReference(1, stack.popReference());
            NEXT();
        }
        OPCODE(ASTORE_2)
        {
            locals.setReference(2, stack.popReference());
            NEXT();
        }
        OPCODE(ASTORE_3)
        {
            locals.setReference(3, stack.popReference());
            NEXT();
        }
        OPCODE(IASTORE)
        {
            Execution::storeIntArrayElement(stack);
            NEXT();
        }
        OPCODE(LASTORE)
        {
            Execution::storeLongArrayElement(stack);
            NEXT();
        }
        OPCODE(FASTORE)
        {
            Execution::storeFloatArrayElement(stack);
            NEXT();
        }
        OPCODE(DASTORE)
        {
            Execution::storeDoubleArrayElement(stack);
            NEXT();
        }
        OPCODE(AASTORE)
        {
            Execution::storeObjectArrayElement(stack);
            NEXT();
        }
        OPCODE(BASTORE)
        {
            Execution::storeIntArrayElement(stack);
            NEXT();
        }
        OPCODE(CASTORE)
        {
            Execution::storeIntArrayElement(stack);
            NEXT();
        }
        OPCODE(SASTORE)
        {
            Execution::storeIntArrayElement(stack);
            NEXT();
        }
        OPCODE(POP)
        {
            stack.dropTop();
            NEXT();
        }
        OPCODE(POP2)
        {
            stack.dropTop();
            stack.dropTop();
            NEXT();
        }
        OPCODE(DUP)
        {
            auto top = stack.popReference();
            stack.pushReference(top);
            stack.pushReference(top);
            NEXT();
        }
        OPCODE(DUP_X1)
        {
            auto v1 = stack.popReference();
            auto v2 = stack.popReference();
            stack.pushReference(v1);
            stack.pushReference(v2);
            stack.pushReference(v1);
            NEXT();
        }
        OPCODE(DUP_X2)
        {
            auto v1 = stack.popReference();
            auto v2 = stack.popReference();
            auto v3 = stack.popReference();
            stack.pushReference(v1);
            stack.pushReference(v3);
            stack.pushReference(v2);
            stack.pushReference(v1);
            NEXT();
        }
        OPCODE(DUP2)
        {
            auto v1 = stack.popReference();
            auto v2 = stack.popReference();
            stack.pushReference(v2);
            stack.pushReference(v1);
            stack.pushReference(v2);
            stack.pushReference(v1);
            NEXT();
        }
        OPCODE(DUP2_X1)
        {
            auto v1 = stack.popReference();
            auto v2 = stack.popReference();
            auto v3 = stack.popReference();
            stack.pushReference(v2);
            stack.pushReference(v1);
            stack.pushReference(v3);
            stack.pushReference(v2);
            stack.pushReference(v1);
            NEXT();
        }
        OPCODE(DUP2_X2)
        {
            auto v1 = stack.popReference();
            auto v2 = stack.popReference();
            auto v3 = stack.popReference();
            auto v4 = stack.popReference();
            stack.pushReference(v2);
            stack.pushReference(v1);
            stack.pushReference(v4);
            stack.pushReference(v3);
            stack.pushReference(v2);
            stack.pushReference(v1);
            NEXT();
        }
        OPCODE(SWAP)
        {
            auto v1 = stack.popReference();
            auto v2 = stack.popReference();
            stack.pushReference(v1);
            stack.pushReference(v2);
            NEXT();
        }
        OPCODE(IADD)
        {
            auto v2 = stack.popInt();
            auto v1 = stack.popInt();
            stack.pushInt(v1 + v2);
            NEXT();
        }
        OPCODE(LADD)
        {
            auto v2 = stack.popLong();
            auto v1 = stack.popLong();
            stack.pushLong(v1 + v2);
            NEXT();
        }
        OPCODE(FADD)
        {
            auto v2 = stack.popFloat();
            auto v1 = stack.popFloat();
            stack.pushFloat(v1 + v2);
            NEXT();
        }
        OPCODE(DADD)
        {
            auto v2 = stack.popDouble();
            auto v1 = stack.popDouble();
            stack.pushDouble(v1 + v2);
            NEXT();
        }
        OPCODE(ISUB)
        {
            auto v2 = stack.popInt();
            auto v1 = stack.popInt();
            stack.pushInt(v1 - v2);
            NEXT();
        }
        OPCODE(LSUB)
        {
            auto v2 = stack.popLong();
            auto v1 = stack.popLong();
            stack.pushLong(v1 - v2);
            NEXT();
        }
        OPCODE(FSUB)
        {
            auto v2 = stack.popFloat();
            auto v1 = stack.popFloat();
            stack.pushFloat(v1 - v2);
            NEXT();
        }
        OPCODE(DSUB)
        {
            auto v2 = stack.popDouble();
            auto v1 = stack.popDouble();
            stack.pushDouble(v1 - v2);
            NEXT();
        }
        OPCODE(IMUL)
        {
            auto v2 = stack.popInt();
            auto v1 = stack.popInt();
            stack.pushInt(v1 * v2);
            NEXT();
        }
        OPCODE(LMUL)
        {
            auto v2 = stack.popLong();
            auto v1 = stack.popLong();
            stack.pushLong(v1 * v2);
            NEXT();
        }
        OPCODE(FMUL)
        {
            auto v2 = stack.popFloat();
            auto v1 = stack.popFloat();
            stack.pushFloat(v1 * v2);
            NEXT();
        }
        OPCODE(DMUL)
        {
            auto v2 = stack.popDouble();
            auto v1 = stack.popDouble();
            stack.pushDouble(v1 * v2);
            NEXT();
        }
        OPCODE(IDIV)
        {
            auto v2 = stack.popInt();
            auto v1 = stack.popInt();
            if (v2 == 0) {
                auto klass = (InstanceKlass* ) BootstrapClassLoader::get()
                    ->loadClass(L"java/lang/ArithmeticException");
                thread->throwException(klass, L"/ by zero");
                stack.clear();
                stack.pushReference(thread->_exceptionOop);
                goto exceptionHandler;
            }
            stack.pushInt(v1 / v2);
            NEXT();
        }
        OPCODE(LDIV)
        {
            auto v2 = stack.popLong();
            auto v1 = stack.popLong();
            if (v2 == 0) {
                auto klass = (InstanceKlass* ) BootstrapClassLoader::get()
                    ->loadClass(L"java/lang/ArithmeticException");
                thread->throwException(klass, L"/ by zero");
                stack.clear();
                stack.pushReference(thread->_exceptionOop);
                goto exceptionHandler;
            }
            stack.pushLong(v1 / v2);
            NEXT();
        }
        OPCODE(FDIV)
        {
            auto v2 = stack.popFloat();
            auto v1 = stack.popFloat();
            if (v2 == 0) {
                auto klass = (InstanceKlass* ) BootstrapClassLoader::get()
                    ->loadClass(L"java/lang/ArithmeticException");
                thread->throwException(klass, L"/ by zero");
                stack.clear();
                stack.pushReference(thread->_exceptionOop);
                goto exceptionHandler;
            }
            stack.pushFloat(v1 / v2);
            NEXT();
        }
        OPCODE(DDIV)
        {
            auto v2 = stack.popDouble();
            auto v1 = stack.popDouble();
            if (v2 == 0) {
                auto klass = (InstanceKlass* ) BootstrapClassLoader::get()
                    ->loadClass(L"java/lang/ArithmeticException");
                thread->throwException(klass, L"/ by zero");
                stack.clear();
                stack.pushReference(thread->_exceptionOop);
                goto exceptionHandler;
            }
            stack.pushDouble(v1 / v2);
            NEXT();
        }
        OPCODE(IREM)
        {
            auto v2 = stack.popInt();
            auto v1 = stack.popInt();
            if (v2 == 0) {
                auto klass = (InstanceKlass* ) BootstrapClassLoader::get()
                    ->loadClass(L"java/lang/ArithmeticException");
                thread->throwException(klass, L"/ by zero");
                stack.clear();
                stack.pushReference(thread->_exceptionOop);
                goto exceptionHandler;
            }
            stack.pushInt(v1 - (v1 / v2) * v2);
            NEXT();
        }
        OPCODE(LREM)
        {
            auto v2 = stack.popLong();
            auto v1 = stack.popLong();
            if (v2 == 0) {
                auto klass = (InstanceKlass* ) BootstrapClassLoader::get()
                    ->loadClass(L"java/lang/ArithmeticException");
                thread->throwException(klass, L"/ by zero");
                stack.clear();
                stack.pushReference(thread->_exceptionOop);
                goto exceptionHandler;
            }
            stack.pushLong(v1 - (v1 / v2) * v2);
            NEXT();
        }
        OPCODE(FREM)
        {
            PANIC("frem is not supported yet");
            NEXT();
        }
        OPCODE(DREM)
        {
            PANIC("drem is not supported yet");
            NEXT();
        }
        OPCODE(INEG)
        {
            stack.pushInt(-stack.popInt());
            NEXT();
        }
        OPCODE(LNEG)
        {
            stack.pushLong(-stack.popLong());
            NEXT();
        }
        OPCODE(FNEG)
        {
            PANIC("fneg is not supported yet");
            NEXT();
        }
        OPCODE(DNEG)
        {
            PANIC("dneg is not supported yet");
            NEXT();
        }
        OPCODE(ISHL)
        {
            auto v2 = stack.popInt();
            auto v1 = stack.popInt();
            auto s = v2 & 0x1F;
            stack.pushInt(v1 << s);
            NEXT();
        }
        OPCODE(LSHL)
        {
            auto v2 = stack.popInt();
            auto v1 = stack.popLong();
            auto s = v2 & 0x3F;
            stack.pushLong(v1 << s);
            NEXT();
        }
        OPCODE(ISHR)
        {
            auto v2 = stack.popInt();
            auto v1 = stack.popInt();
            auto s = v2 & 0x1F;
            stack.pushInt(v1 >> s);
            NEXT();
        }
        OPCODE(LSHR)
        {
            auto v2 = stack.popInt();
            auto v1 = stack.popLong();
            auto s = v2 & 0x3F;
            stack.pushLong(v1 >> s);
            NEXT();
        }
        OPCODE(IUSHR)
        {
            auto v2 = stack.popInt();
            auto v1 = stack.popInt();
            auto s = v2 & 0x1F;
            if (v1 >= 0) {
                stack.pushInt(v1 >> s);
            } else {
                stack.pushInt((v1 >> s) + (2 << ~s));
            }
            NEXT();
        }
        OPCODE(LUSHR)
        {
            auto v2 = stack.popInt();
            auto v1 = stack.popLong();
            auto s = v2 & 0x3F;
            if (v1 >= 0) {
                stack.pushLong(v1 >> s);
            } else {
                stack.pushLong((v1 >> s) + (2 << ~s));
            }
            NEXT();
        }
        OPCODE(IAND)
        {
            auto v2 = stack.popInt();
            auto v1 = stack.popInt();
            stack.pushInt(v1 & v2);
            NEXT();
        }
        OPCODE(LAND)
        {
            auto v2 = stack.popLong();
            auto v1 = stack.popLong();
            stack.pushLong(v1 & v2);
            NEXT();
        }
        OPCODE(IOR)
        {
            auto v2 = stack.popInt();
            auto v1 = stack.popInt();
            stack.pushInt(v1 | v2);
            NEXT();
        }
        OPCODE(LOR)
        {
            auto v2 = stack.popLong();
            auto v1 = stack.popLong();
            stack.pushLong(v1 | v2);
            NEXT();
        }
        OPCODE(IXOR)
        {
            auto v2 = stack.popInt();
            auto v1 = stack.popInt();
            stack.pushInt(v1 ^ v2);
            NEXT();
        }
        OPCODE(LXOR)
        {
            auto v2 = stack.popLong();
            auto v1 = stack.popLong();
            stack.pushLong(v1 ^ v2);
            NEXT();
        }
        OPCODE(IINC)
        {
            int index = codeBlob[pc];
            // the codeBlob stores unsigned char
            int factor = (signed char) codeBlob[pc + 1];
            pc += 2;

            locals.setInt(index, locals.getInt(index) + factor);
            NEXT();
        }
        OPCODE(I2L)
        {
            auto v1 = stack.popInt();
            stack.pushLong((jlong) v1);
            NEXT();
        }
        OPCODE(I2F)
        {
            auto v1 = stack.popInt();
            stack.pushFloat((jfloat) v1);
            NEXT();
        }
        OPCODE(I2D)
        {
            auto v1 = stack.popInt();
            stack.pushDouble((jdouble) v1);
            NEXT();
        }
        OPCODE(L2I)
        {
            auto v1 = stack.popLong();
            stack.pushInt((jint) v1);
            NEXT();
        }
        OPCODE(L2F)
        {
            auto v1 = stack.popLong();
            stack.pushFloat((jfloat) v1);
            NEXT();
        }
        OPCODE(L2D)
        {
            auto v1 = stack.popLong();
            stack.pushDouble((jdouble) v1);
            NEXT();
        }
        OPCODE(F2I)
        {
            auto v1 = stack.popFloat();
            if (FLOAT_IS_NAN(v1)) {
                stack.pushInt(0);
            } else if (FLOAT_IS_POSITIVE_INFINITY(v1)) {
                stack.pushInt(INT_MAX);
            } else if (FLOAT_IS_NEGATIVE_INFINITY(v1)) {
                stack.pushInt(INT_MIN);
            } else {
                stack.pushInt((jint) v1);
            }
            NEXT();
        }
        OPCODE(F2L)
        {
            auto v1 = stack.popFloat();
            if (FLOAT_IS_NAN(v1)) {
                stack.pushLong(0);
            } else if (FLOAT_IS_POSITIVE_INFINITY(v1)) {
                stack.pushLong(LONG_MAX);
            } else if (FLOAT_IS_NEGATIVE_INFINITY(v1)) {
                stack.pushLong(LONG_MIN);
            } else {
                stack.pushLong((jlong) v1);
            }
            NEXT();
        }
        OPCODE(F2D)
        {
            auto v1 = stack.popFloat();
            stack.pushDouble((jdouble) v1);
            NEXT();
        }
        OPCODE(D2I)
        {
            auto v1 = stack.popDouble();
            if (DOUBLE_IS_NAN(v1)) {
                stack.pushInt(0);
            } else if (DOUBLE_IS_POSITIVE_INFINITY(v1)) {
                stack.pushInt(INT_MAX);
            } else if (DOUBLE_IS_NEGATIVE_INFINITY(v1)) {
                stack.pushInt(INT_MIN);
            } else {
                stack.pushInt((jint) v1);
            }
            NEXT();
        }
        OPCODE(D2L)
        {
            auto v1 = stack.popDouble();
            if (DOUBLE_IS_NAN(v1)) {
                stack.pushLong(0);
            } else if (DOUBLE_IS_POSITIVE_INFINITY(v1)) {
                stack.pushLong(LONG_MAX);
            } else if (DOUBLE_IS_NEGATIVE_INFINITY(v1)) {
                stack.pushLong(LONG_MIN);
            } else {
                stack.pushLong((jlong) v1);
            }
            NEXT();
        }
        OPCODE(D2F)
        {
            auto v1 = stack.popDouble();
            stack.pushFloat((jfloat) v1);
            NEXT();
        }
        OPCODE(I2B)
        {
            auto v1 = stack.popInt();
            stack.pushInt((jbyte) v1);
            NEXT();
        }
        OPCODE(I2C)
        {
            auto v1 = stack.popInt();
            stack.pushInt((jchar) v1);
            NEXT();
        }
        OPCODE(I2S)
        {
            auto v1 = stack.popInt();
            stack.pushInt((jshort) v1);
            NEXT();
        }
        OPCODE(LCMP)
        {
            auto v1 = stack.popLong();
            auto v2 = stack.popLong();
            if (v1 > v2) {
                stack.pushInt(-1);
            } else if (v1 < v2) {
                stack.pushInt(1);
            } else {
                stack.pushInt(0);
            }
            NEXT();
        }
        OPCODE(FCMPL)
        {
            auto v1 = stack.popFloat();
            auto v2 = stack.popFloat();
            if (FLOAT_IS_NAN(v1) || FLOAT_IS_NAN(v2)) {
                stack.pushInt(-1);
            } else if (v1 > v2) {
                stack.pushInt(-1);
            } else if (v1 < v2) {
                stack.pushInt(1);
            } else {
                stack.pushInt(0);
            }
            NEXT();
        }
        OPCODE(FCMPG)
        {
            auto v1 = stack.popFloat();
            auto v2 = stack.popFloat();
            if (FLOAT_IS_NAN(v1) || FLOAT_IS_NAN(v2)) {
                stack.pushInt(1);
            } else if (v1 > v2) {
                stack.pushInt(-1);
            } else if (v1 < v2) {
                stack.pushInt(1);
            } else {
                stack.pushInt(0);
            }
            NEXT();
        }
        OPCODE(DCMPL)
        {
            auto v1 = stack.popDouble();
            auto v2 = stack.popDouble();
            if (DOUBLE_IS_NAN(v1) || DOUBLE_IS_NAN(v2)) {
                stack.pushInt(-1);
            } else if (v1 > v2) {
                stack.pushInt(-1);
            } else if (v1 < v2) {
                stack.pushInt(1);
            } else {
                stack.pushInt(0);
            }
            NEXT();
        }
        OPCODE(DCMPG)
        {
            auto v1 = stack.popDouble();
            auto v2 = stack.popDouble();
            if (DOUBLE_IS_NAN(v1) || DOUBLE_IS_NAN(v2)) {
                stack.pushInt(1);
            } else if (v1 > v2) {
                stack.pushInt(-1);
            } else if (v1 < v2) {
                stack.pushInt(1);
            } else {
                stack.pushInt(0);
            }
            NEXT();
        }
        OPCODE(IFEQ)
        {
            IF_GOTO(2, ==);
            NEXT();
        }
        OPCODE(IFNE)
        {
            IF_GOTO(2, !=);
            NEXT();
        }
        OPCODE(IFLT)
        {
            IF_GOTO(2, <);
            NEXT();
        }
        OPCODE(IFGE)
        {
            IF_GOTO(2, >=);
            NEXT();
        }
        OPCODE(IFGT)
        {
            IF_GOTO(2, >);
            NEXT();
        }
        OPCODE(IFLE)
        {
            IF_GOTO(2, <=);
            NEXT();
        }
        OPCODE(IF_ICMPEQ)
        {
            IF_ICMP_GOTO(2, ==);
            NEXT();
        }
        OPCODE(IF_ICMPNE)
        {
            IF_ICMP_GOTO(2, !=);
            NEXT();
        }
        OPCODE(IF_ICMPLT)
        {
            IF_ICMP_GOTO(2, <);
            NEXT();
        }
        OPCODE(IF_ICMPGE)
        {
            IF_ICMP_GOTO(2, >=);
            NEXT();
        }
        OPCODE(IF_ICMPGT)
        {
            IF_ICMP_GOTO(2, >);
            NEXT();
        }
        OPCODE(IF_ICMPLE)
        {
            IF_ICMP_GOTO(2, <=);
            NEXT();
        }
        OPCODE(IF_ACMPEQ)
        {
            IF_ACMP_GOTO(2, ==);
            NEXT();
        }
        OPCODE(IF_ACMPNE)
        {
            IF_ACMP_GOTO(2, !=);
            NEXT();
        }
        OPCODE(GOTO)
        {
            GOTO_BY_OFFSET_HARDCODEDED(2);
            NEXT();
        }
        OPCODE(JSR)
        {
            pc += 2;
            PANIC("jsr should not appear in instructions!");
            NEXT();
        }
        OPCODE(RET)
        {
            pc++;
            PANIC("ret should not appear in instructions!");
            NEXT();
        }
        OPCODE(TABLESWITCH)
        {
            int bc = pc - 1;
            int originBc = bc;
            if (bc % 4 != 0) {
                bc += (4 - bc % 4);
            } else {
                bc += 4;
            }
            int ptr = bc;

            int defaultByte = ((codeBlob[ptr] << 24)
                               | (codeBlob[ptr + 1] << 16)
                               | (codeBlob[ptr + 2] << 8)
                               | (codeBlob[ptr + 3]));
            int lowByte = ((codeBlob[ptr + 4] << 24)
                           | (codeBlob[ptr + 5] << 16)
                           | (codeBlob[ptr + 6] << 8)
                           | (codeBlob[ptr + 7]));
            int highByte = ((codeBlob[ptr + 8] << 24)
                            | (codeBlob[ptr + 9] << 16)
                            | (codeBlob[ptr + 10] << 8)
                            | (codeBlob[ptr + 11]));
            int num = highByte - lowByte + 1;
            ptr += 12;

            // switch-case jump table
            std::vector<int> jumpTable;
            for (int pos = 0; pos < num; pos++) {
                int jump_pos = ((codeBlob[ptr] << 24)
                                | (codeBlob[ptr + 1] << 16)
                                | (codeBlob[ptr + 2] << 8)
                                | (codeBlob[ptr + 3]))
                               + originBc;
                ptr += 4;
                jumpTable.push_back(jump_pos);
            }
            // default
            jumpTable.push_back(defaultByte + originBc);

            int topValue = stack.popInt();
            if (topValue > (int) (jumpTable.size() - 1 + lowByte)
                || topValue < lowByte) {
                // jump to default
                GOTO_ABSOLUTE_WITH_OCCUPIED(static_cast<u4>(jumpTable.back()), 1);
            } else {
                GOTO_ABSOLUTE_WITH_OCCUPIED(static_cast<u4>(jumpTable[topValue - lowByte]), 1);
            }
        }
        NEXT();
        OPCODE(LOOKUPSWITCH)
        {
            int bc = pc - 1;
            int originBc = bc;
            if (bc % 4 != 0) {
                bc += (4 - bc % 4);
            } else {
                bc += 4;
            }
            int ptr = bc;

            int defaultByte = ((codeBlob[ptr] << 24)
                               | (codeBlob[ptr + 1] << 16)
                               | (codeBlob[ptr + 2] << 8)
                               | (codeBlob[ptr + 3]));
            int count = ((codeBlob[ptr + 4] << 24)
                         | (codeBlob[ptr + 5] << 16)
                         | (codeBlob[ptr + 6] << 8)
                         | (codeBlob[ptr + 7]));
            ptr += 8;

            // jump_table
            HashMap<int, int> jumpTable;
            for (int i = 0; i < count; i++) {
                int value = ((codeBlob[ptr] << 24)
                             | (codeBlob[ptr + 1] << 16)
                             | (codeBlob[ptr + 2] << 8)
                             | (codeBlob[ptr + 3]));
                int position = ((codeBlob[ptr + 4] << 24)
                                | (codeBlob[ptr + 5] << 16)
                                | (codeBlob[ptr + 6] << 8)
                                | (codeBlob[ptr + 7]))
                               + originBc;
                ptr += 8;
                jumpTable[value] = position;
            }

            int topValue = stack.popInt();
            auto iter = jumpTable.find(topValue);
            if (iter == jumpTable.end()) {
                GOTO_ABSOLUTE_WITH_OCCUPIED(defaultByte + originBc, 1);
            } else {
                GOTO_ABSOLUTE_WITH_OCCUPIED(iter->second, 1);
            }
        }
        NEXT();
        OPCODE(IRETURN)
        {
            return new intOopDesc(stack.popInt());
            NEXT();
        }
        OPCODE(LRETURN)
        {
            return new longOopDesc(stack.popLong());
            NEXT();
        }
        OPCODE(FRETURN)
        {
            return new floatOopDesc(stack.popFloat());
            NEXT();
        }
        OPCODE(DRETURN)
        {
            return new doubleOopDesc(stack.popDouble());
            NEXT();
        }
        OPCODE(ARETURN)
        {
            return Resolver::javaOop(stack.popReference());
            NEXT();
        }
        OPCODE(RETURN)
        {
            // monitor released in invokeXXX
            return nullptr;
            NEXT();
        }
        OPCODE(GETSTATIC)
        {
            int constantIndex = codeBlob[pc] << 8 | codeBlob[pc + 1];
            pc += 2;
            Execution::getField(thread, currentClass->getRuntimeConstantPool(),
                nullptr, stack, constantIndex);
            NEXT();
        }
        OPCODE(PUTSTATIC)
        {
            int constantIndex = codeBlob[pc] << 8 | codeBlob[pc + 1];
            pc += 2;
            Execution::putField(thread, currentClass->getRuntimeConstantPool(),
                stack, constantIndex, true);
            NEXT();
        }
        OPCODE(GETFIELD)
        {
            int constantIndex = codeBlob[pc] << 8 | codeBlob[pc + 1];
            pc += 2;
            jobject ref = stack.popReference();
            if (ref == nullptr) {
                thread->throwException(Global::java_lang_NullPointerException);
                stack.clear();
                stack.pushReference(thread->_exceptionOop);
                goto exceptionHandler;
            } else {
                instanceOop receiver = Resolver::instance(ref);
                if (receiver == nullptr) {
                    PANIC("Not an instance oop");
                }
                Execution::getField(thread, currentClass->getRuntimeConstantPool(),
                    receiver, stack, constantIndex);
            }
            NEXT();
        }
        OPCODE(PUTFIELD)
        {
            int constantIndex = codeBlob[pc] << 8 | codeBlob[pc + 1];
            pc += 2;
            Execution::putField(thread, currentClass->getRuntimeConstantPool(),
                stack, constantIndex, false);
            NEXT();
        }
        OPCODE(INVOKEVIRTUAL)
        {
            int constantIndex = codeBlob[pc] << 8 | codeBlob[pc + 1];
            pc += 2;
            Execution::invokeVirtual(thread, currentClass->getRuntimeConstantPool(),
                stack, constantIndex);

            if (thread->isExceptionOccurred()) {
                stack.clear();
                stack.pushReference(thread->_exceptionOop);
                goto exceptionHandler;
            }
            NEXT();
        }
        OPCODE(INVOKESPECIAL)
        {
            int constantIndex = codeBlob[pc] << 8 | codeBlob[pc + 1];
            pc += 2;
            Execution::invokeSpecial(thread, currentClass->getRuntimeConstantPool(),
                stack, constantIndex);
            if (thread->isExceptionOccurred()) {
                stack.clear();
                stack.pushReference(thread->_exceptionOop);
                goto exceptionHandler;
            }
            NEXT();
        }
        OPCODE(INVOKESTATIC)
        {
            int constantIndex = codeBlob[pc] << 8 | codeBlob[pc + 1];
            pc += 2;
            Execution::invokeStatic(thread, currentClass->getRuntimeConstantPool(),
                stack, constantIndex);
            if (thread->isExceptionOccurred()) {
                stack.clear();
                stack.pushReference(thread->_exceptionOop);
                goto exceptionHandler;
            }
            NEXT();
        }
        OPCODE(INVOKEINTERFACE)
        {
            int constantIndex = codeBlob[pc] << 8 | codeBlob[pc + 1];
            int count = codeBlob[pc + 2];
            int zero = codeBlob[pc + 3];
            pc += 4;

            if (zero != 0) {
                PANIC("interpreter: invalid invokeinterface: "
                      "the value of the fourth operand byte must always be zero.");
            }
            Execution::invokeInterface(thread, currentClass->getRuntimeConstantPool(),
                stack, constantIndex, count);
            if (thread->isExceptionOccurred()) {
                stack.clear();
                stack.pushReference(thread->_exceptionOop);
                goto exceptionHandler;
            }
            NEXT();
        }
        OPCODE(INVOKEDYNAMIC)
        {
            pc += 4;
            PANIC("invokedynamic");
            NEXT();
        }
        OPCODE(NEW)
        {
            int constantIndex = codeBlob[pc] << 8 | codeBlob[pc + 1];
            pc += 2;
            stack.pushReference(Execution::newInstance(thread, currentClass->getRuntimeConstantPool(),
                constantIndex));
            if (thread->isExceptionOccurred()) {
                stack.clear();
                stack.pushReference(thread->_exceptionOop);
                goto exceptionHandler;
            }
            NEXT();
        }
        OPCODE(NEWARRAY)
        {
            int arrayType = codeBlob[pc++];
            int length = stack.popInt();
            if (length < 0) {
                auto klass = (InstanceKlass *) BootstrapClassLoader::get()
                    ->loadClass(L"java/lang/NegativeArraySizeException");
                thread->throwException(klass, std::to_wstring(length));
                stack.clear();
                stack.pushReference(thread->_exceptionOop);
                goto exceptionHandler;
            }
            auto array = Execution::newPrimitiveArray(thread, arrayType, length);
            stack.pushReference(array);
            NEXT();
        }
        OPCODE(ANEWARRAY)
        {
            int constantIndex = codeBlob[pc] << 8 | codeBlob[pc + 1];
            pc += 2;
            int length = stack.popInt();
            if (length < 0) {
                auto klass = (InstanceKlass *) BootstrapClassLoader::get()
                    ->loadClass(L"java/lang/NegativeArraySizeException");
                thread->throwException(klass, std::to_wstring(length));
                stack.clear();
                stack.pushReference(thread->_exceptionOop);
                goto exceptionHandler;
            }
            auto array = Execution::newObjectArray(thread,
                currentClass->getRuntimeConstantPool(),
                constantIndex, length);
            stack.pushReference(array);
            NEXT();
        }
        OPCODE(ARRAYLENGTH)
        {
            jobject ref = stack.popReference();
            if (ref == nullptr) {
                thread->throwException(Global::java_lang_NullPointerException);
                stack.clear();
                stack.pushReference(thread->_exceptionOop);
                goto exceptionHandler;
            } else {
                arrayOop array = Resolver::array(ref);
                if (array == nullptr) {
                    PANIC("Attempt to use arraylength on non-array objects");
                }
                stack.pushInt(array->getLength());
            }
            NEXT();
        }
        OPCODE(ATHROW)
        {
            exceptionHandler:
            auto exceptionOop = Resolver::instance(stack.popReference());
            if (exceptionOop == nullptr) {
                thread->throwException(Global::java_lang_NullPointerException);
                stack.clear();
                stack.pushReference(thread->_exceptionOop);
                goto exceptionHandler;
            }

            int handler = thread->tryHandleException(exceptionOop);

            if (handler > 0) {
                D("athrow: exception handler found at offset: %d", handler);
                stack.clear();
                stack.pushReference(exceptionOop);
                GOTO_ABSOLUTE(handler);

            } else {
                D("athrow: exception handler not found, rethrowing it to caller");
                return exceptionOop;
            }
            NEXT();
        }
        OPCODE(CHECKCAST)
        {
            int constantIndex = codeBlob[pc] << 8 | codeBlob[pc + 1];
            pc += 2;
            Execution::instanceOf(thread, currentClass->getRuntimeConstantPool(),
                stack, constantIndex, true);
            if (thread->isExceptionOccurred()) {
                stack.clear();
                stack.pushReference(thread->_exceptionOop);
                goto exceptionHandler;
            }
            NEXT();
        }
        OPCODE(INSTANCEOF)
        {
            int constantIndex = codeBlob[pc] << 8 | codeBlob[pc + 1];
            pc += 2;
            Execution::instanceOf(thread, currentClass->getRuntimeConstantPool(),
                stack, constantIndex, false);
            NEXT();
        }
        OPCODE(MONITORENTER)
        {
            jobject ref = stack.popReference();
            if (ref == nullptr) {
                thread->throwException(Global::java_lang_NullPointerException);
                stack.clear();
                stack.pushReference(thread->_exceptionOop);
                goto exceptionHandler;
            } else {
                auto object = Resolver::javaOop(ref);
                if (object == nullptr) {
                    PANIC("not an object");
                }
                object->getMarkOop()->monitorEnter();
            }
            NEXT();
        }
        OPCODE(MONITOREXIT)
        {
            jobject ref = stack.popReference();
            if (ref == nullptr) {
                thread->throwException(Global::java_lang_NullPointerException);
                stack.clear();
                stack.pushReference(thread->_exceptionOop);
                goto exceptionHandler;
            } else {
                auto object = Resolver::javaOop(ref);
                if (object == nullptr) {
                    PANIC("not an object");
                }
                object->getMarkOop()->monitorExit();
            }
            NEXT();
        }
        OPCODE(WIDE)
        {
            PANIC("wide should not appear in instructions!");
            NEXT();
        }
        OPCODE(MULTIANEWARRAY)
        {
            int constantIndex = codeBlob[pc] << 8 | codeBlob[pc + 1];
            int dimension = codeBlob[pc + 2];
            pc += 3;
            std::deque<int> length;
            for (int i = 0; i < dimension; ++i) {
                int sub = stack.popInt();
                if (sub < 0) {
                    auto klass = (InstanceKlass *) BootstrapClassLoader::get()
                        ->loadClass(L"java/lang/NegativeArraySizeException");
                    thread->throwException(klass, std::to_wstring(sub));
                    stack.clear();
                    stack.pushReference(thread->_exceptionOop);
                    goto exceptionHandler;
                }
                length.push_back(sub);
            }
            stack.pushReference(Execution::newMultiObjectArray(thread,
                currentClass->getRuntimeConstantPool(),
                constantIndex, dimension, length));
        }
        NEXT();
        OPCODE(IFNULL)
        {
            IF_NULLCMP_GOTO(2, ==);
            NEXT();
        }
        OPCODE(IFNONNULL)
        {
            IF_NULLCMP_GOTO(2, !=);
            NEXT();
        }
        OPCODE(GOTO_W)
        {
            pc += 4;
            PANIC("goto_w should not appear in instructions!");
            NEXT();
        }
        OPCODE(JSR_W)
        {
            pc += 4;
            PANIC("jsr_w should not appear in instructions!");
            NEXT();
        }
        OTHERWISE()
        {
            PANIC("Unrecognized bytecode: %d at %d", codeBlob[pc - 1], pc - 1);
        }

        return nullptr;
    }
}

#endif
