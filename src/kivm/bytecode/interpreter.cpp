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

        Stack &stack = currentFrame->getStack();
        Locals &locals = currentFrame->getLocals();

        D("currentMethod: %s.%s",
          strings::toStdString(currentClass->getName()).c_str(),
          strings::toStdString(currentMethod->getName()).c_str());

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
                    int index = stack.popInt();
                    jobject ref = stack.popReference();
                    if (ref == nullptr) {
                        // TODO: throw NullPointerException
                        PANIC("java.lang.NullPointerException");
                    }
                    auto array = Resolver::tryResolveTypeArray(ref);
                    if (array == nullptr) {
                        // TODO: throw ClassCastException
                        PANIC("java.lang.ClassCastException");
                    }

                    auto element = (intOop) array->getElementAt(index);
                    stack.pushInt(element->getValue());
                    NEXT();
                }
                OPCODE(LALOAD)
                {
                    int index = stack.popInt();
                    jobject ref = stack.popReference();
                    if (ref == nullptr) {
                        // TODO: throw NullPointerException
                        PANIC("java.lang.NullPointerException");
                    }
                    auto array = Resolver::tryResolveTypeArray(ref);
                    if (array == nullptr) {
                        // TODO: throw ClassCastException
                        PANIC("java.lang.ClassCastException");
                    }

                    auto element = (longOop) array->getElementAt(index);
                    stack.pushLong(element->getValue());
                    NEXT();
                }
                OPCODE(FALOAD)
                {
                    int index = stack.popInt();
                    jobject ref = stack.popReference();
                    if (ref == nullptr) {
                        // TODO: throw NullPointerException
                        PANIC("java.lang.NullPointerException");
                    }
                    auto array = Resolver::tryResolveTypeArray(ref);
                    if (array == nullptr) {
                        // TODO: throw ClassCastException
                        PANIC("java.lang.ClassCastException");
                    }

                    auto element = (floatOop) array->getElementAt(index);
                    stack.pushFloat(element->getValue());
                    NEXT();
                }
                OPCODE(DALOAD)
                {
                    int index = stack.popInt();
                    jobject ref = stack.popReference();
                    if (ref == nullptr) {
                        // TODO: throw NullPointerException
                        PANIC("java.lang.NullPointerException");
                    }
                    auto array = Resolver::tryResolveTypeArray(ref);
                    if (array == nullptr) {
                        // TODO: throw ClassCastException
                        PANIC("java.lang.ClassCastException");
                    }

                    auto element = (doubleOop) array->getElementAt(index);
                    stack.pushDouble(element->getValue());
                    NEXT();
                }
                OPCODE(AALOAD)
                {
                    int index = stack.popInt();
                    jobject ref = stack.popReference();
                    if (ref == nullptr) {
                        // TODO: throw NullPointerException
                        PANIC("java.lang.NullPointerException");
                    }

                    auto array = Resolver::tryResolveObjectArray(ref);
                    if (array == nullptr) {
                        // TODO: throw ClassCastException
                        PANIC("java.lang.ClassCastException");
                    }

                    stack.pushReference(array->getElementAt(index));
                    NEXT();
                }
                OPCODE(BALOAD)
                {
                    NEXT();
                }
                OPCODE(CALOAD)
                {
                    NEXT();
                }
                OPCODE(SALOAD)
                {
                    NEXT();
                }
                OPCODE(ISTORE)
                {
                    pc++;
                    NEXT();
                }
                OPCODE(LSTORE)
                {
                    pc++;
                    NEXT();
                }
                OPCODE(FSTORE)
                {
                    pc++;
                    NEXT();
                }
                OPCODE(DSTORE)
                {
                    pc++;
                    NEXT();
                }
                OPCODE(ASTORE)
                {
                    pc++;
                    NEXT();
                }
                OPCODE(ISTORE_0)
                {
                    NEXT();
                }
                OPCODE(ISTORE_1)
                {
                    NEXT();
                }
                OPCODE(ISTORE_2)
                {
                    NEXT();
                }
                OPCODE(ISTORE_3)
                {
                    NEXT();
                }
                OPCODE(LSTORE_0)
                {
                    NEXT();
                }
                OPCODE(LSTORE_1)
                {
                    NEXT();
                }
                OPCODE(LSTORE_2)
                {
                    NEXT();
                }
                OPCODE(LSTORE_3)
                {
                    NEXT();
                }
                OPCODE(FSTORE_0)
                {
                    NEXT();
                }
                OPCODE(FSTORE_1)
                {
                    NEXT();
                }
                OPCODE(FSTORE_2)
                {
                    NEXT();
                }
                OPCODE(FSTORE_3)
                {
                    NEXT();
                }
                OPCODE(DSTORE_0)
                {
                    NEXT();
                }
                OPCODE(DSTORE_1)
                {
                    NEXT();
                }
                OPCODE(DSTORE_2)
                {
                    NEXT();
                }
                OPCODE(DSTORE_3)
                {
                    NEXT();
                }
                OPCODE(ASTORE_0)
                {
                    NEXT();
                }
                OPCODE(ASTORE_1)
                {
                    NEXT();
                }
                OPCODE(ASTORE_2)
                {
                    NEXT();
                }
                OPCODE(ASTORE_3)
                {
                    NEXT();
                }
                OPCODE(IASTORE)
                {
                    NEXT();
                }
                OPCODE(LASTORE)
                {
                    NEXT();
                }
                OPCODE(FASTORE)
                {
                    NEXT();
                }
                OPCODE(DASTORE)
                {
                    NEXT();
                }
                OPCODE(AASTORE)
                {
                    NEXT();
                }
                OPCODE(BASTORE)
                {
                    NEXT();
                }
                OPCODE(CASTORE)
                {
                    NEXT();
                }
                OPCODE(SASTORE)
                {
                    NEXT();
                }
                OPCODE(POP)
                {
                    NEXT();
                }
                OPCODE(POP2)
                {
                    NEXT();
                }
                OPCODE(DUP)
                {
                    NEXT();
                }
                OPCODE(DUP_X1)
                {
                    NEXT();
                }
                OPCODE(DUP_X2)
                {
                    NEXT();
                }
                OPCODE(DUP2)
                {
                    NEXT();
                }
                OPCODE(DUP2_X1)
                {
                    NEXT();
                }
                OPCODE(DUP2_X2)
                {
                    NEXT();
                }
                OPCODE(SWAP)
                {
                    NEXT();
                }
                OPCODE(IADD)
                {
                    NEXT();
                }
                OPCODE(LADD)
                {
                    NEXT();
                }
                OPCODE(FADD)
                {
                    NEXT();
                }
                OPCODE(DADD)
                {
                    NEXT();
                }
                OPCODE(ISUB)
                {
                    NEXT();
                }
                OPCODE(LSUB)
                {
                    NEXT();
                }
                OPCODE(FSUB)
                {
                    NEXT();
                }
                OPCODE(DSUB)
                {
                    NEXT();
                }
                OPCODE(IMUL)
                {
                    NEXT();
                }
                OPCODE(LMUL)
                {
                    NEXT();
                }
                OPCODE(FMUL)
                {
                    NEXT();
                }
                OPCODE(DMUL)
                {
                    NEXT();
                }
                OPCODE(IDIV)
                {
                    NEXT();
                }
                OPCODE(LDIV)
                {
                    NEXT();
                }
                OPCODE(FDIV)
                {
                    NEXT();
                }
                OPCODE(DDIV)
                {
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
                    NEXT();
                }
                OPCODE(RET)
                {
                    pc++;
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
