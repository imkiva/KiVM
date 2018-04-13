//
// Created by kiva on 2018/3/21.
//

#include <climits>
#include <kivm/bytecode/interpreter.h>
#include <kivm/bytecode/bytecodes.h>
#include <kivm/bytecode/execution.h>
#include <kivm/oop/instanceOop.h>
#include <kivm/oop/arrayOop.h>
#include <kivm/oop/primitiveOop.h>
#include <kivm/oop/mirrorOop.h>
#include <kivm/method.h>
#include <unordered_map>

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

#define GOTO_PC(branch) \
                    pc += branch

#define GOTO_UNCONDITIONALLY() \
                    short branch = code_blob[pc] << 8 | code_blob[pc + 1]; \
                    GOTO_PC(branch)

#define __IF_GOTO_FACTORY(func, target, occupied, op) \
                    if (stack.func() op target) { \
                        GOTO_UNCONDITIONALLY(); \
                    } else { \
                        pc += (occupied); \
                    }

#define __IF_CMP_GOTO_FACTORY(func, occupied, op) \
                    auto v2 = stack.func(); \
                    auto v1 = stack.func(); \
                    if (v1 op v2) { \
                        GOTO_UNCONDITIONALLY(); \
                    } else { \
                        pc += (occupied); \
                    }

#define IF_GOTO(occupied, op) __IF_GOTO_FACTORY(popInt, 0, occupied, op)
#define IF_NULLCMP_GOTO(occupied, op) __IF_GOTO_FACTORY(popReference, nullptr, occupied, op)

#define IF_ICMP_GOTO(occupied, op) __IF_CMP_GOTO_FACTORY(popInt, occupied, op)
#define IF_ACMP_GOTO(occupied, op) __IF_CMP_GOTO_FACTORY(popReference, occupied, op)

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

        if (currentMethod->isSynchronized()) {
            // TODO: enter monitor
        }

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
                    if (v2 == 0) {
                        // TODO: throw java.lang.ArithmeticException
                        PANIC("java.lang.ArithmeticException");
                    }
                    stack.pushInt(v1 / v2);
                    NEXT();
                }
                OPCODE(LDIV)
                {
                    auto v2 = stack.popLong();
                    auto v1 = stack.popLong();
                    if (v2 == 0) {
                        // TODO: throw java.lang.ArithmeticException
                        PANIC("java.lang.ArithmeticException");
                    }
                    stack.pushLong(v1 / v2);
                    NEXT();
                }
                OPCODE(FDIV)
                {
                    auto v2 = stack.popFloat();
                    auto v1 = stack.popFloat();
                    if (v2 == 0) {
                        // TODO: throw java.lang.ArithmeticException
                        PANIC("java.lang.ArithmeticException");
                    }
                    stack.pushFloat(v1 / v2);
                    NEXT();
                }
                OPCODE(DDIV)
                {
                    auto v2 = stack.popDouble();
                    auto v1 = stack.popDouble();
                    if (v2 == 0) {
                        // TODO: throw java.lang.ArithmeticException
                        PANIC("java.lang.ArithmeticException");
                    }
                    stack.pushDouble(v1 / v2);
                    NEXT();
                }
                OPCODE(IREM)
                {
                    auto v2 = stack.popInt();
                    auto v1 = stack.popInt();
                    if (v2 == 0) {
                        // TODO: throw java.lang.ArithmeticException
                        PANIC("java.lang.ArithmeticException");
                    }
                    stack.pushInt(v1 - (v1 / v2) * v2);
                    NEXT();
                }
                OPCODE(LREM)
                {
                    auto v2 = stack.popLong();
                    auto v1 = stack.popLong();
                    if (v2 == 0) {
                        // TODO: throw java.lang.ArithmeticException
                        PANIC("java.lang.ArithmeticException");
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
                    auto v2 = stack.popLong();
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
                    auto v2 = stack.popLong();
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
                    auto v2 = stack.popLong();
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
                    int index = code_blob[pc];
                    int factor = code_blob[pc + 1];
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
                    GOTO_UNCONDITIONALLY();
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
                    int bc = pc;
                    int originBc = bc;
                    if (bc % 4 != 0) {
                        bc += (4 - bc % 4);
                    } else {
                        bc += 4;
                    }
                    int ptr = bc;

                    int defaultByte = ((code_blob[ptr] << 24)
                                       | (code_blob[ptr + 1] << 16)
                                       | (code_blob[ptr + 2] << 8)
                                       | (code_blob[ptr + 3]));
                    int lowByte = ((code_blob[ptr + 4] << 24)
                                   | (code_blob[ptr + 5] << 16)
                                   | (code_blob[ptr + 6] << 8)
                                   | (code_blob[ptr + 7]));
                    int highByte = ((code_blob[ptr + 8] << 24)
                                    | (code_blob[ptr + 9] << 16)
                                    | (code_blob[ptr + 10] << 8)
                                    | (code_blob[ptr + 11]));
                    int num = highByte - lowByte + 1;
                    ptr += 12;

                    // switch-case jump table
                    std::vector<int> jumpTable;
                    for (int pos = 0; pos < num; pos++) {
                        int jump_pos = ((code_blob[ptr] << 24)
                                        | (code_blob[ptr + 1] << 16)
                                        | (code_blob[ptr + 2] << 8)
                                        | (code_blob[ptr + 3]))
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
                        GOTO_PC(static_cast<u4>(jumpTable.back()));
                    } else {
                        GOTO_PC(static_cast<u4>(jumpTable[topValue - lowByte]));
                    }
                    NEXT();
                }
                OPCODE(LOOKUPSWITCH)
                {
                    int bc = pc;
                    int originBc = bc;
                    if (bc % 4 != 0) {
                        bc += (4 - bc % 4);
                    } else {
                        bc += 4;
                    }
                    int ptr = bc;

                    int defaultByte = ((code_blob[ptr] << 24)
                                       | (code_blob[ptr + 1] << 16)
                                       | (code_blob[ptr + 2] << 8)
                                       | (code_blob[ptr + 3]));
                    int count = ((code_blob[ptr + 4] << 24)
                                 | (code_blob[ptr + 5] << 16)
                                 | (code_blob[ptr + 6] << 8)
                                 | (code_blob[ptr + 7]));
                    ptr += 8;

                    // jump_table
                    std::unordered_map<int, int> jumpTable;
                    for (int i = 0; i < count; i++) {
                        int value = ((code_blob[ptr] << 24)
                                     | (code_blob[ptr + 1] << 16)
                                     | (code_blob[ptr + 2] << 8)
                                     | (code_blob[ptr + 3]));
                        int position = ((code_blob[ptr + 4] << 24)
                                        | (code_blob[ptr + 5] << 16)
                                        | (code_blob[ptr + 6] << 8)
                                        | (code_blob[ptr + 7]))
                                       + originBc;
                        ptr += 8;
                        jumpTable[value] = position;
                    }

                    int topValue = stack.popInt();
                    auto iter = jumpTable.find(topValue);
                    if (iter == jumpTable.end()) {
                        GOTO_PC(defaultByte + originBc);
                    } else {
                        GOTO_PC(iter->second);
                    }
                    break;
                    NEXT();
                }
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
                    return Resolver::resolveJObject(stack.popReference());
                    NEXT();
                }
                OPCODE(RETURN)
                {
                    if (currentMethod->isSynchronized()) {
                        // TODO: exit monitor
                    }
                    NEXT();
                }
                OPCODE(GETSTATIC)
                {
                    int constantIndex = code_blob[pc] << 8 | code_blob[pc + 1];
                    pc += 2;
                    Execution::getField(thread, currentClass->getRuntimeConstantPool(),
                                        nullptr, stack, constantIndex);
                    NEXT();
                }
                OPCODE(PUTSTATIC)
                {
                    int constantIndex = code_blob[pc] << 8 | code_blob[pc + 1];
                    pc += 2;
                    Execution::putField(thread, currentClass->getRuntimeConstantPool(),
                                        stack, constantIndex);
                    NEXT();
                }
                OPCODE(GETFIELD)
                {
                    int constantIndex = code_blob[pc] << 8 | code_blob[pc + 1];
                    pc += 2;
                    jobject ref = stack.popReference();
                    if (ref == nullptr) {
                        // TODO: throw NullPointerException
                        PANIC("java.lang.NullPointerException");
                    }
                    instanceOop receiver = Resolver::tryResolveInstance(ref);
                    if (receiver == nullptr) {
                        PANIC("Not an instance oop");
                    }
                    Execution::getField(thread, currentClass->getRuntimeConstantPool(),
                                        receiver, stack, constantIndex);
                    NEXT();
                }
                OPCODE(PUTFIELD)
                {
                    int constantIndex = code_blob[pc] << 8 | code_blob[pc + 1];
                    pc += 2;
                    Execution::putField(thread, currentClass->getRuntimeConstantPool(),
                                        stack, constantIndex);
                    NEXT();
                }
                OPCODE(INVOKEVIRTUAL)
                {
                    pc += 2;
                    PANIC("INVOKEVIRTUAL");
                    NEXT();
                }
                OPCODE(INVOKESPECIAL)
                {
                    pc += 2;
                    PANIC("INVOKESPECIAL");
                    NEXT();
                }
                OPCODE(INVOKESTATIC)
                {
                    pc += 2;
                    PANIC("INVOKESTATIC");
                    NEXT();
                }
                OPCODE(INVOKEINTERFACE)
                {
                    pc += 4;
                    PANIC("INVOKEINTERFACE");
                    NEXT();
                }
                OPCODE(INVOKEDYNAMIC)
                {
                    pc += 4;
                    PANIC("INVOKEDYNAMIC");
                    NEXT();
                }
                OPCODE(NEW)
                {
                    int constantIndex = code_blob[pc] << 8 | code_blob[pc + 1];
                    pc += 2;
                    Execution::newInstance(thread, currentClass->getRuntimeConstantPool(),
                                           stack, constantIndex);
                    NEXT();
                }
                OPCODE(NEWARRAY)
                {
                    int arrayType = code_blob[pc++];
                    int length = stack.popInt();
                    Execution::newPrimitiveArray(thread, stack, arrayType, length, 1);
                    NEXT();
                }
                OPCODE(ANEWARRAY)
                {
                    int constantIndex = code_blob[pc] << 8 | code_blob[pc + 1];
                    pc += 2;
                    int length = stack.popInt();
                    Execution::newObjectArray(thread, currentClass->getRuntimeConstantPool(),
                                              stack, constantIndex, length, 1);
                    NEXT();
                }
                OPCODE(ARRAYLENGTH)
                {
                    jobject ref = stack.popReference();
                    if (ref == nullptr) {
                        // TODO: throw NullPointerException
                        PANIC("java.lang.NullPointerException");
                    }
                    arrayOop array = Resolver::tryResolveArray(ref);
                    if (array == nullptr) {
                        PANIC("Attempt to use arraylength on non-array objects");
                    }
                    stack.pushInt(array->getLength());
                    NEXT();
                }
                OPCODE(ATHROW)
                {
                    PANIC("ATHROW");
                    NEXT();
                }
                OPCODE(CHECKCAST)
                {
                    pc += 2;
                    PANIC("CHECKCAST");
                    NEXT();
                }
                OPCODE(INSTANCEOF)
                {
                    pc += 2;
                    PANIC("INSTANCEOF");
                    NEXT();
                }
                OPCODE(MONITORENTER)
                {
                    PANIC("MONITORENTER");
                    NEXT();
                }
                OPCODE(MONITOREXIT)
                {
                    PANIC("MONITOREXIT");
                    NEXT();
                }
                OPCODE(WIDE)
                {
                    PANIC("WIDE");
                    NEXT();
                }
                OPCODE(MULTIANEWARRAY)
                {
                    PANIC("MULTIANEWARRAY");
                    pc += 3;
                    NEXT();
                }
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
                OTHERWISE() {
                    PANIC("Unrecognized bytecode: %d", code_blob[pc - 1]);
                    NEXT();
                }
            END()

        oop resultOop = nullptr;
        return resultOop;
    }
}
