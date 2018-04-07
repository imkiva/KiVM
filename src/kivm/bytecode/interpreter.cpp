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

        BEGIN(code_blob, pc)

                OPCODE(NOP)
                {
                    NEXT();
                }
                OPCODE(ACONST_NULL)
                {
                    currentFrame->getStack().pushReference(nullptr);
                    NEXT();
                }
                OPCODE(ICONST_M1)
                {
                    currentFrame->getStack().pushInt(-1);
                    NEXT();
                }
                OPCODE(ICONST_0)
                {
                    currentFrame->getStack().pushInt(0);
                    NEXT();
                }
                OPCODE(ICONST_1)
                {
                    currentFrame->getStack().pushInt(1);
                    NEXT();
                }
                OPCODE(ICONST_2)
                {
                    currentFrame->getStack().pushInt(2);
                    NEXT();
                }
                OPCODE(ICONST_3)
                {
                    currentFrame->getStack().pushInt(3);
                    NEXT();
                }
                OPCODE(ICONST_4)
                {
                    currentFrame->getStack().pushInt(4);
                    NEXT();
                }
                OPCODE(ICONST_5)
                {
                    currentFrame->getStack().pushInt(5);
                    NEXT();
                }
                OPCODE(LCONST_0)
                {
                    currentFrame->getStack().pushLong(0);
                    NEXT();
                }
                OPCODE(LCONST_1)
                {
                    currentFrame->getStack().pushLong(1);
                    NEXT();
                }
                OPCODE(FCONST_0)
                {
                    currentFrame->getStack().pushFloat(0);
                    NEXT();
                }
                OPCODE(FCONST_1)
                {
                    currentFrame->getStack().pushFloat(1);
                    NEXT();
                }
                OPCODE(FCONST_2)
                {
                    currentFrame->getStack().pushFloat(2);
                    NEXT();
                }
                OPCODE(DCONST_0)
                {
                    currentFrame->getStack().pushDouble(0);
                    NEXT();
                }
                OPCODE(DCONST_1)
                {
                    currentFrame->getStack().pushDouble(1);
                    NEXT();
                }
                OPCODE(BIPUSH)
                {
                    currentFrame->getStack().pushInt(code_blob[pc++]);
                    NEXT();
                }
                OPCODE(SIPUSH)
                {
                    short si = code_blob[pc] << 8 | code_blob[pc + 1];
                    pc += 2;
                    currentFrame->getStack().pushInt(si);
                    NEXT();
                }
                OPCODE(LDC)
                {
                    int constantIndex = code_blob[pc++];
                    Execution::loadConstant(currentClass->getRuntimeConstantPool(),
                                            currentFrame->getStack(), constantIndex);
                    NEXT();
                }
                OPCODE(LDC_W)
                {
                    int constantIndex = code_blob[pc] << 8 | code_blob[pc + 1];
                    pc += 2;
                    Execution::loadConstant(currentClass->getRuntimeConstantPool(),
                                            currentFrame->getStack(), constantIndex);
                    NEXT();
                }
                OPCODE(LDC2_W)
                {
                    int constantIndex = code_blob[pc] << 8 | code_blob[pc + 1];
                    pc += 2;
                    Execution::loadConstant(currentClass->getRuntimeConstantPool(),
                                            currentFrame->getStack(), constantIndex);
                    NEXT();
                }
                OPCODE(ILOAD)
                {
                    int index = code_blob[pc++];
                    currentFrame->getStack().pushInt(currentFrame->getLocals().getInt(index));
                    NEXT();
                }
                OPCODE(LLOAD)
                {
                    int index = code_blob[pc++];
                    currentFrame->getStack().pushLong(currentFrame->getLocals().getLong(index));
                    NEXT();
                }
                OPCODE(FLOAD)
                {
                    int index = code_blob[pc++];
                    currentFrame->getStack().pushFloat(currentFrame->getLocals().getFloat(index));
                    NEXT();
                }
                OPCODE(DLOAD)
                {
                    int index = code_blob[pc++];
                    currentFrame->getStack().pushDouble(currentFrame->getLocals().getDouble(index));
                    NEXT();
                }
                OPCODE(ALOAD)
                {
                    int index = code_blob[pc++];
                    currentFrame->getStack().pushReference(currentFrame->getLocals().getReference(index));
                    NEXT();
                }
                OPCODE(ILOAD_0)
                {
                    currentFrame->getStack().pushInt(currentFrame->getLocals().getInt(0));
                    NEXT();
                }
                OPCODE(ILOAD_1)
                {
                    currentFrame->getStack().pushInt(currentFrame->getLocals().getInt(1));
                    NEXT();
                }
                OPCODE(ILOAD_2)
                {
                    currentFrame->getStack().pushInt(currentFrame->getLocals().getInt(2));
                    NEXT();
                }
                OPCODE(ILOAD_3)
                {
                    currentFrame->getStack().pushInt(currentFrame->getLocals().getInt(3));
                    NEXT();
                }
                OPCODE(LLOAD_0)
                {
                    currentFrame->getStack().pushLong(currentFrame->getLocals().getLong(0));
                    NEXT();
                }
                OPCODE(LLOAD_1)
                {
                    currentFrame->getStack().pushLong(currentFrame->getLocals().getLong(1));
                    NEXT();
                }
                OPCODE(LLOAD_2)
                {
                    currentFrame->getStack().pushLong(currentFrame->getLocals().getLong(2));
                    NEXT();
                }
                OPCODE(LLOAD_3)
                {
                    currentFrame->getStack().pushLong(currentFrame->getLocals().getLong(3));
                    NEXT();
                }
                OPCODE(FLOAD_0)
                {
                    currentFrame->getStack().pushFloat(currentFrame->getLocals().getFloat(0));
                    NEXT();
                }
                OPCODE(FLOAD_1)
                {
                    currentFrame->getStack().pushFloat(currentFrame->getLocals().getFloat(1));
                    NEXT();
                }
                OPCODE(FLOAD_2)
                {
                    currentFrame->getStack().pushFloat(currentFrame->getLocals().getFloat(2));
                    NEXT();
                }
                OPCODE(FLOAD_3)
                {
                    currentFrame->getStack().pushFloat(currentFrame->getLocals().getFloat(3));
                    NEXT();
                }
                OPCODE(DLOAD_0)
                {
                    currentFrame->getStack().pushDouble(currentFrame->getLocals().getDouble(0));
                    NEXT();
                }
                OPCODE(DLOAD_1)
                {
                    currentFrame->getStack().pushDouble(currentFrame->getLocals().getDouble(1));
                    NEXT();
                }
                OPCODE(DLOAD_2)
                {
                    currentFrame->getStack().pushDouble(currentFrame->getLocals().getDouble(2));
                    NEXT();
                }
                OPCODE(DLOAD_3)
                {
                    currentFrame->getStack().pushDouble(currentFrame->getLocals().getDouble(3));
                    NEXT();
                }
                OPCODE(ALOAD_0)
                {
                    currentFrame->getStack().pushReference(currentFrame->getLocals().getReference(0));
                    NEXT();
                }
                OPCODE(ALOAD_1)
                {
                    currentFrame->getStack().pushReference(currentFrame->getLocals().getReference(1));
                    NEXT();
                }
                OPCODE(ALOAD_2)
                {
                    currentFrame->getStack().pushReference(currentFrame->getLocals().getReference(2));
                    NEXT();
                }
                OPCODE(ALOAD_3)
                {
                    currentFrame->getStack().pushReference(currentFrame->getLocals().getReference(3));
                    NEXT();
                }
                OPCODE(IALOAD)
                {
                    int index = currentFrame->getStack().popInt();
                    jobject ref = currentFrame->getStack().popReference();
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
                    currentFrame->getStack().pushInt(element->getValue());
                    NEXT();
                }
                OPCODE(LALOAD)
                {
                    int index = currentFrame->getStack().popInt();
                    jobject ref = currentFrame->getStack().popReference();
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
                    currentFrame->getStack().pushLong(element->getValue());
                    NEXT();
                }
                OPCODE(FALOAD)
                {
                    int index = currentFrame->getStack().popInt();
                    jobject ref = currentFrame->getStack().popReference();
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
                    currentFrame->getStack().pushFloat(element->getValue());
                    NEXT();
                }
                OPCODE(DALOAD)
                {
                    int index = currentFrame->getStack().popInt();
                    jobject ref = currentFrame->getStack().popReference();
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
                    currentFrame->getStack().pushDouble(element->getValue());
                    NEXT();
                }
                OPCODE(AALOAD)
                {
                    int index = currentFrame->getStack().popInt();
                    jobject ref = currentFrame->getStack().popReference();
                    if (ref == nullptr) {
                        // TODO: throw NullPointerException
                        PANIC("java.lang.NullPointerException");
                    }

                    auto array = Resolver::tryResolveObjectArray(ref);
                    if (array == nullptr) {
                        // TODO: throw ClassCastException
                        PANIC("java.lang.ClassCastException");
                    }

                    currentFrame->getStack().pushReference(array->getElementAt(index));
                    NEXT();
                }
                OPCODE(BALOAD)
                {
                    int index = currentFrame->getStack().popInt();
                    jobject ref = currentFrame->getStack().popReference();
                    NEXT();
                }
                OPCODE(CALOAD)
                {
                    int index = currentFrame->getStack().popInt();
                    jobject ref = currentFrame->getStack().popReference();
                    NEXT();
                }
                OPCODE(SALOAD)
                {
                    int index = currentFrame->getStack().popInt();
                    jobject ref = currentFrame->getStack().popReference();
                    NEXT();
                }
                OPCODE(ISTORE)
                {
                    int localIndex = pc++;
                    NEXT();
                }
                OPCODE(LSTORE)
                {
                    int localIndex = pc++;
                    NEXT();
                }
                OPCODE(FSTORE)
                {
                    int localIndex = pc++;
                    NEXT();
                }
                OPCODE(DSTORE)
                {
                    int localIndex = pc++;
                    NEXT();
                }
                OPCODE(ASTORE)
                {
                    int localIndex = pc++;
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
                    jobject value = currentFrame->getStack().popReference();
                    int index = currentFrame->getStack().popInt();
                    jobject ref = currentFrame->getStack().popReference();
                    NEXT();
                }
                OPCODE(LASTORE)
                {
                    jobject value = currentFrame->getStack().popReference();
                    int index = currentFrame->getStack().popInt();
                    jobject ref = currentFrame->getStack().popReference();
                    NEXT();
                }
                OPCODE(FASTORE)
                {
                    jobject value = currentFrame->getStack().popReference();
                    int index = currentFrame->getStack().popInt();
                    jobject ref = currentFrame->getStack().popReference();
                    NEXT();
                }
                OPCODE(DASTORE)
                {
                    jobject value = currentFrame->getStack().popReference();
                    int index = currentFrame->getStack().popInt();
                    jobject ref = currentFrame->getStack().popReference();
                    NEXT();
                }
                OPCODE(AASTORE)
                {
                    jobject value = currentFrame->getStack().popReference();
                    int index = currentFrame->getStack().popInt();
                    jobject ref = currentFrame->getStack().popReference();
                    NEXT();
                }
                OPCODE(BASTORE)
                {
                    jobject value = currentFrame->getStack().popReference();
                    int index = currentFrame->getStack().popInt();
                    jobject ref = currentFrame->getStack().popReference();
                    NEXT();
                }
                OPCODE(CASTORE)
                {
                    jobject value = currentFrame->getStack().popReference();
                    int index = currentFrame->getStack().popInt();
                    jobject ref = currentFrame->getStack().popReference();
                    NEXT();
                }
                OPCODE(SASTORE)
                {
                    jobject value = currentFrame->getStack().popReference();
                    int index = currentFrame->getStack().popInt();
                    jobject ref = currentFrame->getStack().popReference();
                    NEXT();
                }
                OPCODE(POP)
                {
                    currentFrame->getStack().dropTop();
                    NEXT();
                }
                OPCODE(POP2)
                {
                    currentFrame->getStack().dropTop();
                    currentFrame->getStack().dropTop();
                    NEXT();
                }
                OPCODE(DUP)
                {
                    currentFrame->getStack().dup();
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
