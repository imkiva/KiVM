//
// Created by kiva on 2018/3/21.
//

#include <kivm/bytecode/interpreter.h>
#include <kivm/bytecode/bytecodes.h>
#include <kivm/method.h>

#define BEGIN(code, pc) \
    while ((code).validate_offset(pc)) \
        switch ((code)[(pc)++]) {
#define OPCODE(opcode) \
    case OPC_##opcode:
#define OTHERWISE() \
    default:
#define NEXT() break
#define END() }

namespace kivm {
    oop ByteCodeInterpreter::interp(JavaThread *thread) {
        Frame *frame = thread->get_current_frame();
        const CodeBlob &code_blob = frame->get_method()->get_code_blob();
        u4 &pc = thread->_pc;

        BEGIN(code_blob, pc)

                OPCODE(NOP) {
                    NEXT();
                }
                OPCODE(ACONST_NULL) {
                    NEXT();
                }
                OPCODE(ICONST_M1) {
                    NEXT();
                }
                OPCODE(ICONST_0) {
                    NEXT();
                }
                OPCODE(ICONST_1) {
                    NEXT();
                }
                OPCODE(ICONST_2) {
                    NEXT();
                }
                OPCODE(ICONST_3) {
                    NEXT();
                }
                OPCODE(ICONST_4) {
                    NEXT();
                }
                OPCODE(ICONST_5) {
                    NEXT();
                }
                OPCODE(LCONST_0) {
                    NEXT();
                }
                OPCODE(LCONST_1) {
                    NEXT();
                }
                OPCODE(FCONST_0) {
                    NEXT();
                }
                OPCODE(FCONST_1) {
                    NEXT();
                }
                OPCODE(FCONST_2) {
                    NEXT();
                }
                OPCODE(DCONST_0) {
                    NEXT();
                }
                OPCODE(DCONST_1) {
                    NEXT();
                }
                OPCODE(BIPUSH) {
                    NEXT();
                }
                OPCODE(SIPUSH) {
                    NEXT();
                }
                OPCODE(LDC) {
                    NEXT();
                }
                OPCODE(LDC_W) {
                    NEXT();
                }
                OPCODE(LDC2_W) {
                    NEXT();
                }
                OPCODE(ILOAD) {
                    NEXT();
                }
                OPCODE(LLOAD) {
                    NEXT();
                }
                OPCODE(FLOAD) {
                    NEXT();
                }
                OPCODE(DLOAD) {
                    NEXT();
                }
                OPCODE(ALOAD) {
                    NEXT();
                }
                OPCODE(ILOAD_0) {
                    NEXT();
                }
                OPCODE(ILOAD_1) {
                    NEXT();
                }
                OPCODE(ILOAD_2) {
                    NEXT();
                }
                OPCODE(ILOAD_3) {
                    NEXT();
                }
                OPCODE(LLOAD_0) {
                    NEXT();
                }
                OPCODE(LLOAD_1) {
                    NEXT();
                }
                OPCODE(LLOAD_2) {
                    NEXT();
                }
                OPCODE(LLOAD_3) {
                    NEXT();
                }
                OPCODE(FLOAD_0) {
                    NEXT();
                }
                OPCODE(FLOAD_1) {
                    NEXT();
                }
                OPCODE(FLOAD_2) {
                    NEXT();
                }
                OPCODE(FLOAD_3) {
                    NEXT();
                }
                OPCODE(DLOAD_0) {
                    NEXT();
                }
                OPCODE(DLOAD_1) {
                    NEXT();
                }
                OPCODE(DLOAD_2) {
                    NEXT();
                }
                OPCODE(DLOAD_3) {
                    NEXT();
                }
                OPCODE(ALOAD_0) {
                    NEXT();
                }
                OPCODE(ALOAD_1) {
                    NEXT();
                }
                OPCODE(ALOAD_2) {
                    NEXT();
                }
                OPCODE(ALOAD_3) {
                    NEXT();
                }
                OPCODE(IALOAD) {
                    NEXT();
                }
                OPCODE(LALOAD) {
                    NEXT();
                }
                OPCODE(FALOAD) {
                    NEXT();
                }
                OPCODE(DALOAD) {
                    NEXT();
                }
                OPCODE(AALOAD) {
                    NEXT();
                }
                OPCODE(BALOAD) {
                    NEXT();
                }
                OPCODE(CALOAD) {
                    NEXT();
                }
                OPCODE(SALOAD) {
                    NEXT();
                }
                OPCODE(ISTORE) {
                    NEXT();
                }
                OPCODE(LSTORE) {
                    NEXT();
                }
                OPCODE(FSTORE) {
                    NEXT();
                }
                OPCODE(DSTORE) {
                    NEXT();
                }
                OPCODE(ASTORE) {
                    NEXT();
                }
                OPCODE(ISTORE_0) {
                    NEXT();
                }
                OPCODE(ISTORE_1) {
                    NEXT();
                }
                OPCODE(ISTORE_2) {
                    NEXT();
                }
                OPCODE(ISTORE_3) {
                    NEXT();
                }
                OPCODE(LSTORE_0) {
                    NEXT();
                }
                OPCODE(LSTORE_1) {
                    NEXT();
                }
                OPCODE(LSTORE_2) {
                    NEXT();
                }
                OPCODE(LSTORE_3) {
                    NEXT();
                }
                OPCODE(FSTORE_0) {
                    NEXT();
                }
                OPCODE(FSTORE_1) {
                    NEXT();
                }
                OPCODE(FSTORE_2) {
                    NEXT();
                }
                OPCODE(FSTORE_3) {
                    NEXT();
                }
                OPCODE(DSTORE_0) {
                    NEXT();
                }
                OPCODE(DSTORE_1) {
                    NEXT();
                }
                OPCODE(DSTORE_2) {
                    NEXT();
                }
                OPCODE(DSTORE_3) {
                    NEXT();
                }
                OPCODE(ASTORE_0) {
                    NEXT();
                }
                OPCODE(ASTORE_1) {
                    NEXT();
                }
                OPCODE(ASTORE_2) {
                    NEXT();
                }
                OPCODE(ASTORE_3) {
                    NEXT();
                }
                OPCODE(IASTORE) {
                    NEXT();
                }
                OPCODE(LASTORE) {
                    NEXT();
                }
                OPCODE(FASTORE) {
                    NEXT();
                }
                OPCODE(DASTORE) {
                    NEXT();
                }
                OPCODE(AASTORE) {
                    NEXT();
                }
                OPCODE(BASTORE) {
                    NEXT();
                }
                OPCODE(CASTORE) {
                    NEXT();
                }
                OPCODE(SASTORE) {
                    NEXT();
                }
                OPCODE(POP) {
                    NEXT();
                }
                OPCODE(POP2) {
                    NEXT();
                }
                OPCODE(DUP) {
                    NEXT();
                }
                OPCODE(DUP_X1) {
                    NEXT();
                }
                OPCODE(DUP_X2) {
                    NEXT();
                }
                OPCODE(DUP2) {
                    NEXT();
                }
                OPCODE(DUP2_X1) {
                    NEXT();
                }
                OPCODE(DUP2_X2) {
                    NEXT();
                }
                OPCODE(SWAP) {
                    NEXT();
                }
                OPCODE(IADD) {
                    NEXT();
                }
                OPCODE(LADD) {
                    NEXT();
                }
                OPCODE(FADD) {
                    NEXT();
                }
                OPCODE(DADD) {
                    NEXT();
                }
                OPCODE(ISUB) {
                    NEXT();
                }
                OPCODE(LSUB) {
                    NEXT();
                }
                OPCODE(FSUB) {
                    NEXT();
                }
                OPCODE(DSUB) {
                    NEXT();
                }
                OPCODE(IMUL) {
                    NEXT();
                }
                OPCODE(LMUL) {
                    NEXT();
                }
                OPCODE(FMUL) {
                    NEXT();
                }
                OPCODE(DMUL) {
                    NEXT();
                }
                OPCODE(IDIV) {
                    NEXT();
                }
                OPCODE(LDIV) {
                    NEXT();
                }
                OPCODE(FDIV) {
                    NEXT();
                }
                OPCODE(DDIV) {
                    NEXT();
                }
                OPCODE(IREM) {
                    NEXT();
                }
                OPCODE(LREM) {
                    NEXT();
                }
                OPCODE(FREM) {
                    NEXT();
                }
                OPCODE(DREM) {
                    NEXT();
                }
                OPCODE(INEG) {
                    NEXT();
                }
                OPCODE(LNEG) {
                    NEXT();
                }
                OPCODE(FNEG) {
                    NEXT();
                }
                OPCODE(DNEG) {
                    NEXT();
                }
                OPCODE(ISHL) {
                    NEXT();
                }
                OPCODE(LSHL) {
                    NEXT();
                }
                OPCODE(ISHR) {
                    NEXT();
                }
                OPCODE(LSHR) {
                    NEXT();
                }
                OPCODE(IUSHR) {
                    NEXT();
                }
                OPCODE(LUSHR) {
                    NEXT();
                }
                OPCODE(IAND) {
                    NEXT();
                }
                OPCODE(LAND) {
                    NEXT();
                }
                OPCODE(IOR) {
                    NEXT();
                }
                OPCODE(LOR) {
                    NEXT();
                }
                OPCODE(IXOR) {
                    NEXT();
                }
                OPCODE(LXOR) {
                    NEXT();
                }
                OPCODE(IINC) {
                    NEXT();
                }
                OPCODE(I2L) {
                    NEXT();
                }
                OPCODE(I2F) {
                    NEXT();
                }
                OPCODE(I2D) {
                    NEXT();
                }
                OPCODE(L2I) {
                    NEXT();
                }
                OPCODE(L2F) {
                    NEXT();
                }
                OPCODE(L2D) {
                    NEXT();
                }
                OPCODE(F2I) {
                    NEXT();
                }
                OPCODE(F2L) {
                    NEXT();
                }
                OPCODE(F2D) {
                    NEXT();
                }
                OPCODE(D2I) {
                    NEXT();
                }
                OPCODE(D2L) {
                    NEXT();
                }
                OPCODE(D2F) {
                    NEXT();
                }
                OPCODE(I2B) {
                    NEXT();
                }
                OPCODE(I2C) {
                    NEXT();
                }
                OPCODE(I2S) {
                    NEXT();
                }
                OPCODE(LCMP) {
                    NEXT();
                }
                OPCODE(FCMPL) {
                    NEXT();
                }
                OPCODE(FCMPG) {
                    NEXT();
                }
                OPCODE(DCMPL) {
                    NEXT();
                }
                OPCODE(DCMPG) {
                    NEXT();
                }
                OPCODE(IFEQ) {
                    NEXT();
                }
                OPCODE(IFNE) {
                    NEXT();
                }
                OPCODE(IFLT) {
                    NEXT();
                }
                OPCODE(IFGE) {
                    NEXT();
                }
                OPCODE(IFGT) {
                    NEXT();
                }
                OPCODE(IFLE) {
                    NEXT();
                }
                OPCODE(IF_ICMPEQ) {
                    NEXT();
                }
                OPCODE(IF_ICMPNE) {
                    NEXT();
                }
                OPCODE(IF_ICMPLT) {
                    NEXT();
                }
                OPCODE(IF_ICMPGE) {
                    NEXT();
                }
                OPCODE(IF_ICMPGT) {
                    NEXT();
                }
                OPCODE(IF_ICMPLE) {
                    NEXT();
                }
                OPCODE(IF_ACMPEQ) {
                    NEXT();
                }
                OPCODE(IF_ACMPNE) {
                    NEXT();
                }
                OPCODE(GOTO) {
                    NEXT();
                }
                OPCODE(JSR) {
                    NEXT();
                }
                OPCODE(RET) {
                    NEXT();
                }
                OPCODE(TABLESWITCH) {
                    NEXT();
                }
                OPCODE(LOOKUPSWITCH) {
                    NEXT();
                }
                OPCODE(IRETURN) {
                    NEXT();
                }
                OPCODE(LRETURN) {
                    NEXT();
                }
                OPCODE(FRETURN) {
                    NEXT();
                }
                OPCODE(DRETURN) {
                    NEXT();
                }
                OPCODE(ARETURN) {
                    NEXT();
                }
                OPCODE(RETURN) {
                    NEXT();
                }
                OPCODE(GETSTATIC) {
                    NEXT();
                }
                OPCODE(PUTSTATIC) {
                    NEXT();
                }
                OPCODE(GETFIELD) {
                    NEXT();
                }
                OPCODE(PUTFIELD) {
                    NEXT();
                }
                OPCODE(INVOKEVIRTUAL) {
                    NEXT();
                }
                OPCODE(INVOKESPECIAL) {
                    NEXT();
                }
                OPCODE(INVOKESTATIC) {
                    NEXT();
                }
                OPCODE(INVOKEINTERFACE) {
                    NEXT();
                }
                OPCODE(INVOKEDYNAMIC) {
                    NEXT();
                }
                OPCODE(NEW) {
                    NEXT();
                }
                OPCODE(NEWARRAY) {
                    NEXT();
                }
                OPCODE(ANEWARRAY) {
                    NEXT();
                }
                OPCODE(ARRAYLENGTH) {
                    NEXT();
                }
                OPCODE(ATHROW) {
                    NEXT();
                }
                OPCODE(CHECKCAST) {
                    NEXT();
                }
                OPCODE(INSTANCEOF) {
                    NEXT();
                }
                OPCODE(MONITORENTER) {
                    NEXT();
                }
                OPCODE(MONITOREXIT) {
                    NEXT();
                }
                OPCODE(WIDE) {
                    NEXT();
                }
                OPCODE(MULTIANEWARRAY) {
                    NEXT();
                }
                OPCODE(IFNULL) {
                    NEXT();
                }
                OPCODE(IFNONNULL) {
                    NEXT();
                }
                OPCODE(GOTO_W) {
                    NEXT();
                }
                OPCODE(JSR_W) {
                    NEXT();
                }
                OPCODE(LDC_QUICK) {
                    NEXT();
                }
                OPCODE(LDC_W_QUICK) {
                    NEXT();
                }
                OPCODE(GETFIELD_QUICK) {
                    NEXT();
                }
                OPCODE(PUTFIELD_QUICK) {
                    NEXT();
                }
                OPCODE(GETFIELD2_QUICK) {
                    NEXT();
                }
                OPCODE(PUTFIELD2_QUICK) {
                    NEXT();
                }
                OPCODE(GETSTATIC_QUICK) {
                    NEXT();
                }
                OPCODE(PUTSTATIC_QUICK) {
                    NEXT();
                }
                OPCODE(GETSTATIC2_QUICK) {
                    NEXT();
                }
                OPCODE(PUTSTATIC2_QUICK) {
                    NEXT();
                }
                OPCODE(INVOKEVIRTUAL_QUICK) {
                    NEXT();
                }
                OPCODE(INVOKENONVIRTUAL_QUICK) {
                    NEXT();
                }
                OPCODE(INVOKESUPER_QUICK) {
                    NEXT();
                }
                OPCODE(GETFIELD_QUICK_REF) {
                    NEXT();
                }
                OPCODE(PUTFIELD_QUICK_REF) {
                    NEXT();
                }
                OPCODE(GETSTATIC_QUICK_REF) {
                    NEXT();
                }
                OPCODE(PUTSTATIC_QUICK_REF) {
                    NEXT();
                }
                OPCODE(GETFIELD_THIS_REF) {
                    NEXT();
                }
                OPCODE(MIRANDA_BRIDGE) {
                    NEXT();
                }
                OPCODE(ABSTRACT_METHOD_ERROR) {
                    NEXT();
                }
                OPCODE(INLINE_REWRITER) {
                    NEXT();
                }
                OPCODE(PROFILE_REWRITER) {
                    NEXT();
                }
                OPCODE(INVOKEVIRTUAL_QUICK_W) {
                    NEXT();
                }
                OPCODE(GETFIELD_QUICK_W) {
                    NEXT();
                }
                OPCODE(PUTFIELD_QUICK_W) {
                    NEXT();
                }
                OPCODE(GETFIELD_THIS) {
                    NEXT();
                }
                OPCODE(LOCK) {
                    NEXT();
                }
                OPCODE(ALOAD_THIS) {
                    NEXT();
                }
                OPCODE(INVOKESTATIC_QUICK) {
                    NEXT();
                }
                OPCODE(NEW_QUICK) {
                    NEXT();
                }
                OPCODE(ANEWARRAY_QUICK) {
                    NEXT();
                }
                OPCODE(CHECKCAST_QUICK) {
                    NEXT();
                }
                OPCODE(INSTANCEOF_QUICK) {
                    NEXT();
                }
                OPCODE(MULTIANEWARRAY_QUICK) {
                    NEXT();
                }
                OPCODE(INVOKEHANDLE) {
                    NEXT();
                }
                OPCODE(INVOKEBASIC) {
                    NEXT();
                }
                OPCODE(LINKTOSPECIAL) {
                    NEXT();
                }
                OPCODE(LINKTOVIRTUAL) {
                    NEXT();
                }
                OPCODE(LINKTOINTERFACE) {
                    NEXT();
                }
                OPCODE(INVOKEINTERFACE_QUICK) {
                    NEXT();
                }
                OPCODE(INVOKEDYNAMIC_QUICK) {
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
