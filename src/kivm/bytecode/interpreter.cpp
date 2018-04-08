//
// Created by kiva on 2018/3/21.
//

#include <kivm/bytecode/interpreter.h>
#include <kivm/bytecode/bytecodes.h>
#include <kivm/bytecode/execution.h>
#include <kivm/oop/instanceOop.h>
#include <kivm/oop/arrayOop.h>
#include <kivm/oop/primitiveOop.h>
#include <kivm/oop/mirrorOop.h>
#include <kivm/method.h>

#define OPCODE_DEBUG

#define BEGIN(code, pc) \
    while ((code).validateOffset(pc)) \
        switch ((code)[(pc)++]) {
#define OTHERWISE() \
    default:
#define NEXT() break
#define END() }

#ifdef OPCODE_DEBUG
#define OPCODE(opcode) \
    case OPC_##opcode: \
        D("pc: %d, opcode: %d, name: %s", pc, code_blob[pc - 1], #opcode);
#else
#define OPCODE(opcode) \
    case OPC_##opcode:
#endif

namespace kivm {
    oop ByteCodeInterpreter::interp(JavaThread *thread) {
        Frame *currentFrame = thread->getCurrentFrame();
        auto currentMethod = currentFrame->getMethod();
        auto currentClass = currentMethod->getClass();
        const CodeBlob &code_blob = currentMethod->getCodeBlob();
        u4 &pc = thread->_pc;

        D("currentMethod: %s.%s:%s",
          strings::toStdString(currentClass->getName()).c_str(),
          strings::toStdString(currentMethod->getName()).c_str(),
          strings::toStdString(currentMethod->getDescriptor()).c_str());

        Stack &stack = currentFrame->getStack();
        Locals &locals = currentFrame->getLocals();

        BEGIN(code_blob, pc)

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
                    stack.pushInt(code_blob[pc++]);
                    NEXT();
                }
                OPCODE(SIPUSH)
                {
                    short si = code_blob[pc] << 8 | code_blob[pc + 1];
                    pc += 2;
                    stack.pushInt(si);
                    NEXT();
                }
                OPCODE(LDC)
                {
                    int constantIndex = code_blob[pc++];
                    Execution::loadConstant(currentClass->getRuntimeConstantPool(),
                                            stack, constantIndex);
                    NEXT();
                }
                OPCODE(LDC_W)
                {
                    int constantIndex = code_blob[pc] << 8 | code_blob[pc + 1];
                    pc += 2;
                    Execution::loadConstant(currentClass->getRuntimeConstantPool(),
                                            stack, constantIndex);
                    NEXT();
                }
                OPCODE(LDC2_W)
                {
                    int constantIndex = code_blob[pc] << 8 | code_blob[pc + 1];
                    pc += 2;
                    Execution::loadConstant(currentClass->getRuntimeConstantPool(),
                                            stack, constantIndex);
                    NEXT();
                }
                OPCODE(ILOAD)
                {
                    int index = code_blob[pc++];
                    stack.pushInt(locals.getInt(index));
                    NEXT();
                }
                OPCODE(LLOAD)
                {
                    int index = code_blob[pc++];
                    stack.pushLong(locals.getLong(index));
                    NEXT();
                }
                OPCODE(FLOAD)
                {
                    int index = code_blob[pc++];
                    stack.pushFloat(locals.getFloat(index));
                    NEXT();
                }
                OPCODE(DLOAD)
                {
                    int index = code_blob[pc++];
                    stack.pushDouble(locals.getDouble(index));
                    NEXT();
                }
                OPCODE(ALOAD)
                {
                    int index = code_blob[pc++];
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
                {
                    Execution::loadIntArrayElement(stack);
                    NEXT();
                }
                OPCODE(LALOAD)
                {
                    Execution::loadLongArrayElement(stack);
                    NEXT();
                }
                OPCODE(FALOAD)
                {
                    Execution::loadFloatArrayElement(stack);
                    NEXT();
                }
                OPCODE(DALOAD)
                {
                    Execution::loadDoubleArrayElement(stack);
                    NEXT();
                }
                OPCODE(AALOAD)
                {
                    Execution::loadObjectArrayElement(stack);
                    NEXT();
                }
                OPCODE(BALOAD)
                {
                    Execution::loadIntArrayElement(stack);
                    NEXT();
                }
                OPCODE(CALOAD)
                {
                    Execution::loadIntArrayElement(stack);
                    NEXT();
                }
                OPCODE(SALOAD)
                {
                    Execution::loadIntArrayElement(stack);
                    NEXT();
                }
                OPCODE(ISTORE)
                {
                    int localIndex = pc++;
                    stack.pushInt(locals.getInt(localIndex));
                    NEXT();
                }
                OPCODE(LSTORE)
                {
                    int localIndex = pc++;
                    stack.pushLong(locals.getLong(localIndex));
                    NEXT();
                }
                OPCODE(FSTORE)
                {
                    int localIndex = pc++;
                    stack.pushFloat(locals.getFloat(localIndex));
                    NEXT();
                }
                OPCODE(DSTORE)
                {
                    int localIndex = pc++;
                    stack.pushDouble(locals.getDouble(localIndex));
                    NEXT();
                }
                OPCODE(ASTORE)
                {
                    int localIndex = pc++;
                    stack.pushReference(locals.getReference(localIndex));
                    NEXT();
                }
                OPCODE(ISTORE_0)
                {
                    stack.pushInt(locals.getInt(0));
                    NEXT();
                }
                OPCODE(ISTORE_1)
                {
                    stack.pushInt(locals.getInt(1));
                    NEXT();
                }
                OPCODE(ISTORE_2)
                {
                    stack.pushInt(locals.getInt(2));
                    NEXT();
                }
                OPCODE(ISTORE_3)
                {
                    stack.pushInt(locals.getInt(3));
                    NEXT();
                }
                OPCODE(LSTORE_0)
                {
                    stack.pushLong(locals.getLong(0));
                    NEXT();
                }
                OPCODE(LSTORE_1)
                {
                    stack.pushLong(locals.getLong(1));
                    NEXT();
                }
                OPCODE(LSTORE_2)
                {
                    stack.pushLong(locals.getLong(2));
                    NEXT();
                }
                OPCODE(LSTORE_3)
                {
                    stack.pushLong(locals.getLong(3));
                    NEXT();
                }
                OPCODE(FSTORE_0)
                {
                    stack.pushFloat(locals.getFloat(0));
                    NEXT();
                }
                OPCODE(FSTORE_1)
                {
                    stack.pushFloat(locals.getFloat(1));
                    NEXT();
                }
                OPCODE(FSTORE_2)
                {
                    stack.pushFloat(locals.getFloat(2));
                    NEXT();
                }
                OPCODE(FSTORE_3)
                {
                    stack.pushFloat(locals.getFloat(3));
                    NEXT();
                }
                OPCODE(DSTORE_0)
                {
                    stack.pushDouble(locals.getDouble(0));
                    NEXT();
                }
                OPCODE(DSTORE_1)
                {
                    stack.pushDouble(locals.getDouble(1));
                    NEXT();
                }
                OPCODE(DSTORE_2)
                {
                    stack.pushDouble(locals.getDouble(2));
                    NEXT();
                }
                OPCODE(DSTORE_3)
                {
                    stack.pushDouble(locals.getDouble(3));
                    NEXT();
                }
                OPCODE(ASTORE_0)
                {
                    stack.pushReference(locals.getReference(0));
                    NEXT();
                }
                OPCODE(ASTORE_1)
                {
                    stack.pushReference(locals.getReference(1));
                    NEXT();
                }
                OPCODE(ASTORE_2)
                {
                    stack.pushReference(locals.getReference(2));
                    NEXT();
                }
                OPCODE(ASTORE_3)
                {
                    stack.pushReference(locals.getReference(3));
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
                    stack.pushInt(v1 / v2);
                    NEXT();
                }
                OPCODE(LDIV)
                {
                    auto v2 = stack.popLong();
                    auto v1 = stack.popLong();
                    stack.pushLong(v1 / v2);
                    NEXT();
                }
                OPCODE(FDIV)
                {
                    auto v2 = stack.popFloat();
                    auto v1 = stack.popFloat();
                    stack.pushFloat(v1 / v2);
                    NEXT();
                }
                OPCODE(DDIV)
                {
                    auto v2 = stack.popDouble();
                    auto v1 = stack.popDouble();
                    stack.pushDouble(v1 / v2);
                    NEXT();
                }
                OPCODE(IREM)
                {
                    NEXT();
                }
                OPCODE(LREM)
                {
                    NEXT();
                }
                OPCODE(FREM)
                {
                    NEXT();
                }
                OPCODE(DREM)
                {
                    NEXT();
                }
                OPCODE(INEG)
                {
                    NEXT();
                }
                OPCODE(LNEG)
                {
                    NEXT();
                }
                OPCODE(FNEG)
                {
                    NEXT();
                }
                OPCODE(DNEG)
                {
                    NEXT();
                }
                OPCODE(ISHL)
                {
                    NEXT();
                }
                OPCODE(LSHL)
                {
                    NEXT();
                }
                OPCODE(ISHR)
                {
                    NEXT();
                }
                OPCODE(LSHR)
                {
                    NEXT();
                }
                OPCODE(IUSHR)
                {
                    NEXT();
                }
                OPCODE(LUSHR)
                {
                    NEXT();
                }
                OPCODE(IAND)
                {
                    NEXT();
                }
                OPCODE(LAND)
                {
                    NEXT();
                }
                OPCODE(IOR)
                {
                    NEXT();
                }
                OPCODE(LOR)
                {
                    NEXT();
                }
                OPCODE(IXOR)
                {
                    NEXT();
                }
                OPCODE(LXOR)
                {
                    NEXT();
                }
                OPCODE(IINC)
                {
                    pc += 2;
                    NEXT();
                }
                OPCODE(I2L)
                {
                    NEXT();
                }
                OPCODE(I2F)
                {
                    NEXT();
                }
                OPCODE(I2D)
                {
                    NEXT();
                }
                OPCODE(L2I)
                {
                    NEXT();
                }
                OPCODE(L2F)
                {
                    NEXT();
                }
                OPCODE(L2D)
                {
                    NEXT();
                }
                OPCODE(F2I)
                {
                    NEXT();
                }
                OPCODE(F2L)
                {
                    NEXT();
                }
                OPCODE(F2D)
                {
                    NEXT();
                }
                OPCODE(D2I)
                {
                    NEXT();
                }
                OPCODE(D2L)
                {
                    NEXT();
                }
                OPCODE(D2F)
                {
                    NEXT();
                }
                OPCODE(I2B)
                {
                    NEXT();
                }
                OPCODE(I2C)
                {
                    NEXT();
                }
                OPCODE(I2S)
                {
                    NEXT();
                }
                OPCODE(LCMP)
                {
                    NEXT();
                }
                OPCODE(FCMPL)
                {
                    NEXT();
                }
                OPCODE(FCMPG)
                {
                    NEXT();
                }
                OPCODE(DCMPL)
                {
                    NEXT();
                }
                OPCODE(DCMPG)
                {
                    NEXT();
                }
                OPCODE(IFEQ)
                {
                    pc += 2;
                    NEXT();
                }
                OPCODE(IFNE)
                {
                    pc += 2;
                    NEXT();
                }
                OPCODE(IFLT)
                {
                    pc += 2;
                    NEXT();
                }
                OPCODE(IFGE)
                {
                    pc += 2;
                    NEXT();
                }
                OPCODE(IFGT)
                {
                    pc += 2;
                    NEXT();
                }
                OPCODE(IFLE)
                {
                    pc += 2;
                    NEXT();
                }
                OPCODE(IF_ICMPEQ)
                {
                    pc += 2;
                    NEXT();
                }
                OPCODE(IF_ICMPNE)
                {
                    pc += 2;
                    NEXT();
                }
                OPCODE(IF_ICMPLT)
                {
                    pc += 2;
                    NEXT();
                }
                OPCODE(IF_ICMPGE)
                {
                    pc += 2;
                    NEXT();
                }
                OPCODE(IF_ICMPGT)
                {
                    pc += 2;
                    NEXT();
                }
                OPCODE(IF_ICMPLE)
                {
                    pc += 2;
                    NEXT();
                }
                OPCODE(IF_ACMPEQ)
                {
                    pc += 2;
                    NEXT();
                }
                OPCODE(IF_ACMPNE)
                {
                    pc += 2;
                    NEXT();
                }
                OPCODE(GOTO)
                {
                    pc += 2;
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
                    // var args
                    NEXT();
                }
                OPCODE(LOOKUPSWITCH)
                {
                    // var args
                    NEXT();
                }
                OPCODE(IRETURN)
                {
                    NEXT();
                }
                OPCODE(LRETURN)
                {
                    NEXT();
                }
                OPCODE(FRETURN)
                {
                    NEXT();
                }
                OPCODE(DRETURN)
                {
                    NEXT();
                }
                OPCODE(ARETURN)
                {
                    NEXT();
                }
                OPCODE(RETURN)
                {
                    NEXT();
                }
                OPCODE(GETSTATIC)
                {
                    pc += 2;
                    NEXT();
                }
                OPCODE(PUTSTATIC)
                {
                    pc += 2;
                    NEXT();
                }
                OPCODE(GETFIELD)
                {
                    pc += 2;
                    NEXT();
                }
                OPCODE(PUTFIELD)
                {
                    pc += 2;
                    NEXT();
                }
                OPCODE(INVOKEVIRTUAL)
                {
                    pc += 2;
                    NEXT();
                }
                OPCODE(INVOKESPECIAL)
                {
                    pc += 2;
                    NEXT();
                }
                OPCODE(INVOKESTATIC)
                {
                    pc += 2;
                    NEXT();
                }
                OPCODE(INVOKEINTERFACE)
                {
                    pc += 4;
                    NEXT();
                }
                OPCODE(INVOKEDYNAMIC)
                {
                    pc += 4;
                    NEXT();
                }
                OPCODE(NEW)
                {
                    pc += 2;
                    NEXT();
                }
                OPCODE(NEWARRAY)
                {
                    pc++;
                    NEXT();
                }
                OPCODE(ANEWARRAY)
                {
                    pc += 2;
                    NEXT();
                }
                OPCODE(ARRAYLENGTH)
                {
                    NEXT();
                }
                OPCODE(ATHROW)
                {
                    NEXT();
                }
                OPCODE(CHECKCAST)
                {
                    pc += 2;
                    NEXT();
                }
                OPCODE(INSTANCEOF)
                {
                    pc += 2;
                    NEXT();
                }
                OPCODE(MONITORENTER)
                {
                    NEXT();
                }
                OPCODE(MONITOREXIT)
                {
                    NEXT();
                }
                OPCODE(WIDE)
                {
                    NEXT();
                }
                OPCODE(MULTIANEWARRAY)
                {
                    pc += 3;
                    NEXT();
                }
                OPCODE(IFNULL)
                {
                    pc += 2;
                    NEXT();
                }
                OPCODE(IFNONNULL)
                {
                    pc += 2;
                    NEXT();
                }
                OPCODE(GOTO_W)
                {
                    pc += 4;
                    NEXT();
                }
                OPCODE(JSR_W)
                {
                    pc += 4;
                    PANIC("jsr_w should not appear in instructions!");
                    NEXT();
                }
                OTHERWISE() {
                    PANIC("Unrecognized bytecode: %d", code_blob[pc - 1]);
                    NEXT();
                }
            END()

        oop resultOop = nullptr;
        return resultOop;
    }
}
