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
#pragma clang diagnostic push
#pragma ide diagnostic ignored "OCDFAInspection"
    class ScratchInterpreter {
    public:
        /**
         * Run a thread method
         *
         * @param thread Java Thread that contains method
         * @return method return value(nullptr if void) o
         *         exception object(if thrown and not handled)
         */
        static oop interp(JavaThread *thread);
    };
#pragma clang diagnostic pop

    oop ScratchInterpreter::interp(JavaThread *thread) {
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

        BEGIN(codeBlob, pc)

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

                OPCODE(BASTORE)
                OPCODE(CASTORE)
                OPCODE(SASTORE)
                OPCODE(IASTORE)
                {
                    STORE_ARRAY_ELEMENT(value, typeArray, popInt, new intOopDesc(value));
                    NEXT();
                }

                OPCODE(LASTORE)
                {
                    STORE_ARRAY_ELEMENT(value, typeArray, popLong, new longOopDesc(value));
                    NEXT();
                }
                OPCODE(FASTORE)
                {
                    STORE_ARRAY_ELEMENT(value, typeArray, popFloat, new floatOopDesc(value));
                    NEXT();
                }
                OPCODE(DASTORE)
                {
                    STORE_ARRAY_ELEMENT(value, typeArray, popDouble, new doubleOopDesc(value));
                    NEXT();
                }
                OPCODE(AASTORE)
                {
                    STORE_ARRAY_ELEMENT(value, objectArray, popReference, Resolver::javaOop(value));
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
                    PANIC("Use of deprecated instruction frem, please check your Java compiler");
                    NEXT();
                }
                OPCODE(DREM)
                {
                    PANIC("Use of deprecated instruction drem, please check your Java compiler");
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
                    PANIC("Use of deprecated instruction fneg, please check your Java compiler");
                    NEXT();
                }
                OPCODE(DNEG)
                {
                    PANIC("Use of deprecated instruction dneg, please check your Java compiler");
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
                    PANIC("Use of deprecated instruction jsr, please check your Java compiler");
                    NEXT();
                }
                OPCODE(RET)
                {
                    pc++;
                    PANIC("Use of deprecated instruction ret, please check your Java compiler");
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
                    if (thread->isExceptionOccurred()) {
                        stack.clear();
                        stack.pushReference(thread->_exceptionOop);
                        goto exceptionHandler;
                    }
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
                            // TODO: throw an exception
                            SHOULD_NOT_REACH_HERE_M("Not an instance oop");
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
                    if (thread->isExceptionOccurred()) {
                        stack.clear();
                        stack.pushReference(thread->_exceptionOop);
                        goto exceptionHandler;
                    }
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
                        WARN("interpreter: invalid invokeinterface: "
                              "the value of the fourth operand byte must always be zero.");
                        // continue
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
                    int constantIndex = codeBlob[pc] << 8 | codeBlob[pc + 1];
                    if (codeBlob[pc + 2] != 0 || codeBlob[pc + 3] != 0) {
                        WARN("interpreter: invalid invokedynamic: "
                              "the value of the third and fourth operand bytes must always be zero.");
                        // continue
                    }
                    pc += 4;

                    Execution::invokeDynamic(thread, currentClass, stack, constantIndex);
                    if (thread->isExceptionOccurred()) {
                        stack.clear();
                        stack.pushReference(thread->_exceptionOop);
                        goto exceptionHandler;
                    }
                    NEXT();
                }
                OPCODE(NEW)
                {
                    int constantIndex = codeBlob[pc] << 8 | codeBlob[pc + 1];
                    pc += 2;
                    auto instance = Execution::newInstance(thread, currentClass->getRuntimeConstantPool(),
                        constantIndex);
                    if (thread->isExceptionOccurred()) {
                        stack.clear();
                        stack.pushReference(thread->_exceptionOop);
                        goto exceptionHandler;
                    }
                    stack.pushReference(instance);
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
                            // TODO: throw an exception
                            SHOULD_NOT_REACH_HERE_M("Attempt to use arraylength on non-array objects");
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
                            // TODO: throw an exception
                            SHOULD_NOT_REACH_HERE_M("not an object");
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
                            // TODO: throw an exception
                            SHOULD_NOT_REACH_HERE_M("not an object");
                        }
                        object->getMarkOop()->monitorExit();
                    }
                    NEXT();
                }
                OPCODE(WIDE)
                {
                    PANIC("Use of deprecated instruction wide, please check your Java compiler");
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
                    PANIC("Use of deprecated instruction goto_w, please check your Java compiler");
                    NEXT();
                }
                OPCODE(JSR_W)
                {
                    pc += 4;
                    PANIC("Use of deprecated instruction jsr_w, please check your Java compiler");
                    NEXT();
                }
                OTHERWISE() {
                    PANIC("Use of undefined bytecode: %d at %d", codeBlob[pc - 1], pc - 1);
                    NEXT();
                }
            END()

        return nullptr;
    }
}
