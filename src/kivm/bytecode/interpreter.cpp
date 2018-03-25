//
// Created by kiva on 2018/3/21.
//

#include <kivm/bytecode/interpreter.h>

#define THREADED

#ifdef THREADED
#define OPCODE_BEGIN(jump_table, pc, ...) \
        void *jump_table[] = { __VA_ARGS__ }; \
        goto *jump_table[pc++]
#define OPCODE_END()
#define LABEL(x)        label_##x
#define NEXT(jump_table, pc)    goto *jump_table[pc++]
#endif

namespace kivm {
    oop ByteCodeInterpreter::interp(JavaThread *thread) {
        int pc = 0;

#ifdef THREADED
#define IMPL(x)         &&LABEL(x)
#endif

        OPCODE_BEGIN(labels, pc,
                     IMPL(OPC_NOP),
                     IMPL(OPC_ACONST_NULL),
                     IMPL(OPC_ICONST_M1),
                     IMPL(OPC_ICONST_0),
                     IMPL(OPC_ICONST_1),
                     IMPL(OPC_ICONST_2),
                     IMPL(OPC_ICONST_3),
                     IMPL(OPC_ICONST_4),
                     IMPL(OPC_ICONST_5),
                     IMPL(OPC_LCONST_0),
                     IMPL(OPC_LCONST_1),
                     IMPL(OPC_FCONST_0),
                     IMPL(OPC_FCONST_1),
                     IMPL(OPC_FCONST_2),
                     IMPL(OPC_DCONST_0),
                     IMPL(OPC_DCONST_1),
                     IMPL(OPC_BIPUSH),
                     IMPL(OPC_SIPUSH),
                     IMPL(OPC_LDC),
                     IMPL(OPC_LDC_W),
                     IMPL(OPC_LDC2_W),
                     IMPL(OPC_ILOAD),
                     IMPL(OPC_LLOAD),
                     IMPL(OPC_FLOAD),
                     IMPL(OPC_DLOAD),
                     IMPL(OPC_ALOAD),
                     IMPL(OPC_ILOAD_0),
                     IMPL(OPC_ILOAD_1),
                     IMPL(OPC_ILOAD_2),
                     IMPL(OPC_ILOAD_3),
                     IMPL(OPC_LLOAD_0),
                     IMPL(OPC_LLOAD_1),
                     IMPL(OPC_LLOAD_2),
                     IMPL(OPC_LLOAD_3),
                     IMPL(OPC_FLOAD_0),
                     IMPL(OPC_FLOAD_1),
                     IMPL(OPC_FLOAD_2),
                     IMPL(OPC_FLOAD_3),
                     IMPL(OPC_DLOAD_0),
                     IMPL(OPC_DLOAD_1),
                     IMPL(OPC_DLOAD_2),
                     IMPL(OPC_DLOAD_3),
                     IMPL(OPC_ALOAD_0),
                     IMPL(OPC_ALOAD_1),
                     IMPL(OPC_ALOAD_2),
                     IMPL(OPC_ALOAD_3),
                     IMPL(OPC_IALOAD),
                     IMPL(OPC_LALOAD),
                     IMPL(OPC_FALOAD),
                     IMPL(OPC_DALOAD),
                     IMPL(OPC_AALOAD),
                     IMPL(OPC_BALOAD),
                     IMPL(OPC_CALOAD),
                     IMPL(OPC_SALOAD),
                     IMPL(OPC_ISTORE),
                     IMPL(OPC_LSTORE),
                     IMPL(OPC_FSTORE),
                     IMPL(OPC_DSTORE),
                     IMPL(OPC_ASTORE),
                     IMPL(OPC_ISTORE_0),
                     IMPL(OPC_ISTORE_1),
                     IMPL(OPC_ISTORE_2),
                     IMPL(OPC_ISTORE_3),
                     IMPL(OPC_LSTORE_0),
                     IMPL(OPC_LSTORE_1),
                     IMPL(OPC_LSTORE_2),
                     IMPL(OPC_LSTORE_3),
                     IMPL(OPC_FSTORE_0),
                     IMPL(OPC_FSTORE_1),
                     IMPL(OPC_FSTORE_2),
                     IMPL(OPC_FSTORE_3),
                     IMPL(OPC_DSTORE_0),
                     IMPL(OPC_DSTORE_1),
                     IMPL(OPC_DSTORE_2),
                     IMPL(OPC_DSTORE_3),
                     IMPL(OPC_ASTORE_0),
                     IMPL(OPC_ASTORE_1),
                     IMPL(OPC_ASTORE_2),
                     IMPL(OPC_ASTORE_3),
                     IMPL(OPC_IASTORE),
                     IMPL(OPC_LASTORE),
                     IMPL(OPC_FASTORE),
                     IMPL(OPC_DASTORE),
                     IMPL(OPC_AASTORE),
                     IMPL(OPC_BASTORE),
                     IMPL(OPC_CASTORE),
                     IMPL(OPC_SASTORE),
                     IMPL(OPC_POP),
                     IMPL(OPC_POP2),
                     IMPL(OPC_DUP),
                     IMPL(OPC_DUP_X1),
                     IMPL(OPC_DUP_X2),
                     IMPL(OPC_DUP2),
                     IMPL(OPC_DUP2_X1),
                     IMPL(OPC_DUP2_X2),
                     IMPL(OPC_SWAP),
                     IMPL(OPC_IADD),
                     IMPL(OPC_LADD),
                     IMPL(OPC_FADD),
                     IMPL(OPC_DADD),
                     IMPL(OPC_ISUB),
                     IMPL(OPC_LSUB),
                     IMPL(OPC_FSUB),
                     IMPL(OPC_DSUB),
                     IMPL(OPC_IMUL),
                     IMPL(OPC_LMUL),
                     IMPL(OPC_FMUL),
                     IMPL(OPC_DMUL),
                     IMPL(OPC_IDIV),
                     IMPL(OPC_LDIV),
                     IMPL(OPC_FDIV),
                     IMPL(OPC_DDIV),
                     IMPL(OPC_IREM),
                     IMPL(OPC_LREM),
                     IMPL(OPC_FREM),
                     IMPL(OPC_DREM),
                     IMPL(OPC_INEG),
                     IMPL(OPC_LNEG),
                     IMPL(OPC_FNEG),
                     IMPL(OPC_DNEG),
                     IMPL(OPC_ISHL),
                     IMPL(OPC_LSHL),
                     IMPL(OPC_ISHR),
                     IMPL(OPC_LSHR),
                     IMPL(OPC_IUSHR),
                     IMPL(OPC_LUSHR),
                     IMPL(OPC_IAND),
                     IMPL(OPC_LAND),
                     IMPL(OPC_IOR),
                     IMPL(OPC_LOR),
                     IMPL(OPC_IXOR),
                     IMPL(OPC_LXOR),
                     IMPL(OPC_IINC),
                     IMPL(OPC_I2L),
                     IMPL(OPC_I2F),
                     IMPL(OPC_I2D),
                     IMPL(OPC_L2I),
                     IMPL(OPC_L2F),
                     IMPL(OPC_L2D),
                     IMPL(OPC_F2I),
                     IMPL(OPC_F2L),
                     IMPL(OPC_F2D),
                     IMPL(OPC_D2I),
                     IMPL(OPC_D2L),
                     IMPL(OPC_D2F),
                     IMPL(OPC_I2B),
                     IMPL(OPC_I2C),
                     IMPL(OPC_I2S),
                     IMPL(OPC_LCMP),
                     IMPL(OPC_FCMPL),
                     IMPL(OPC_FCMPG),
                     IMPL(OPC_DCMPL),
                     IMPL(OPC_DCMPG),
                     IMPL(OPC_IFEQ),
                     IMPL(OPC_IFNE),
                     IMPL(OPC_IFLT),
                     IMPL(OPC_IFGE),
                     IMPL(OPC_IFGT),
                     IMPL(OPC_IFLE),
                     IMPL(OPC_IF_ICMPEQ),
                     IMPL(OPC_IF_ICMPNE),
                     IMPL(OPC_IF_ICMPLT),
                     IMPL(OPC_IF_ICMPGE),
                     IMPL(OPC_IF_ICMPGT),
                     IMPL(OPC_IF_ICMPLE),
                     IMPL(OPC_IF_ACMPEQ),
                     IMPL(OPC_IF_ACMPNE),
                     IMPL(OPC_GOTO),
                     IMPL(OPC_JSR),
                     IMPL(OPC_RET),
                     IMPL(OPC_TABLESWITCH),
                     IMPL(OPC_LOOKUPSWITCH),
                     IMPL(OPC_IRETURN),
                     IMPL(OPC_LRETURN),
                     IMPL(OPC_FRETURN),
                     IMPL(OPC_DRETURN),
                     IMPL(OPC_ARETURN),
                     IMPL(OPC_RETURN),
                     IMPL(OPC_GETSTATIC),
                     IMPL(OPC_PUTSTATIC),
                     IMPL(OPC_GETFIELD),
                     IMPL(OPC_PUTFIELD),
                     IMPL(OPC_INVOKEVIRTUAL),
                     IMPL(OPC_INVOKESPECIAL),
                     IMPL(OPC_INVOKESTATIC),
                     IMPL(OPC_INVOKEINTERFACE),
                     IMPL(OPC_INVOKEDYNAMIC),
                     IMPL(OPC_NEW),
                     IMPL(OPC_NEWARRAY),
                     IMPL(OPC_ANEWARRAY),
                     IMPL(OPC_ARRAYLENGTH),
                     IMPL(OPC_ATHROW),
                     IMPL(OPC_CHECKCAST),
                     IMPL(OPC_INSTANCEOF),
                     IMPL(OPC_MONITORENTER),
                     IMPL(OPC_MONITOREXIT),
                     IMPL(OPC_WIDE),
                     IMPL(OPC_MULTIANEWARRAY),
                     IMPL(OPC_IFNULL),
                     IMPL(OPC_IFNONNULL),
                     IMPL(OPC_GOTO_W),
                     IMPL(OPC_JSR_W),
                     IMPL(OPC_LDC_QUICK),
                     IMPL(OPC_LDC_W_QUICK),
                     IMPL(OPC_GETFIELD_QUICK),
                     IMPL(OPC_PUTFIELD_QUICK),
                     IMPL(OPC_GETFIELD2_QUICK),
                     IMPL(OPC_PUTFIELD2_QUICK),
                     IMPL(OPC_GETSTATIC_QUICK),
                     IMPL(OPC_PUTSTATIC_QUICK),
                     IMPL(OPC_GETSTATIC2_QUICK),
                     IMPL(OPC_PUTSTATIC2_QUICK),
                     IMPL(OPC_INVOKEVIRTUAL_QUICK),
                     IMPL(OPC_INVOKENONVIRTUAL_QUICK),
                     IMPL(OPC_INVOKESUPER_QUICK),
                     IMPL(OPC_GETFIELD_QUICK_REF),
                     IMPL(OPC_PUTFIELD_QUICK_REF),
                     IMPL(OPC_GETSTATIC_QUICK_REF),
                     IMPL(OPC_PUTSTATIC_QUICK_REF),
                     IMPL(OPC_GETFIELD_THIS_REF),
                     IMPL(OPC_MIRANDA_BRIDGE),
                     IMPL(OPC_ABSTRACT_METHOD_ERROR),
                     IMPL(OPC_INLINE_REWRITER),
                     IMPL(OPC_PROFILE_REWRITER),
                     IMPL(OPC_INVOKEVIRTUAL_QUICK_W),
                     IMPL(OPC_GETFIELD_QUICK_W),
                     IMPL(OPC_PUTFIELD_QUICK_W),
                     IMPL(OPC_GETFIELD_THIS),
                     IMPL(OPC_LOCK),
                     IMPL(OPC_ALOAD_THIS),
                     IMPL(OPC_INVOKESTATIC_QUICK),
                     IMPL(OPC_NEW_QUICK),
                     IMPL(OPC_ANEWARRAY_QUICK),
                     IMPL(OPC_CHECKCAST_QUICK),
                     IMPL(OPC_INSTANCEOF_QUICK),
                     IMPL(OPC_MULTIANEWARRAY_QUICK),
                     IMPL(OPC_INVOKEHANDLE),
                     IMPL(OPC_INVOKEBASIC),
                     IMPL(OPC_LINKTOSPECIAL),
                     IMPL(OPC_LINKTOVIRTUAL),
                     IMPL(OPC_LINKTOINTERFACE),
                     IMPL(OPC_INVOKEINTERFACE_QUICK),
                     IMPL(OPC_INVOKEDYNAMIC_QUICK)
        );

#ifdef THREADED
#undef IMPL
#define IMPL(x) LABEL(x):
#endif

        IMPL(OPC_NOP)
        { NEXT(labels, pc); }
        IMPL(OPC_ACONST_NULL)
        { NEXT(labels, pc); }
        IMPL(OPC_ICONST_M1)
        { NEXT(labels, pc); }
        IMPL(OPC_ICONST_0)
        { NEXT(labels, pc); }
        IMPL(OPC_ICONST_1)
        { NEXT(labels, pc); }
        IMPL(OPC_ICONST_2)
        { NEXT(labels, pc); }
        IMPL(OPC_ICONST_3)
        { NEXT(labels, pc); }
        IMPL(OPC_ICONST_4)
        { NEXT(labels, pc); }
        IMPL(OPC_ICONST_5)
        { NEXT(labels, pc); }
        IMPL(OPC_LCONST_0)
        { NEXT(labels, pc); }
        IMPL(OPC_LCONST_1)
        { NEXT(labels, pc); }
        IMPL(OPC_FCONST_0)
        { NEXT(labels, pc); }
        IMPL(OPC_FCONST_1)
        { NEXT(labels, pc); }
        IMPL(OPC_FCONST_2)
        { NEXT(labels, pc); }
        IMPL(OPC_DCONST_0)
        { NEXT(labels, pc); }
        IMPL(OPC_DCONST_1)
        { NEXT(labels, pc); }
        IMPL(OPC_BIPUSH)
        { NEXT(labels, pc); }
        IMPL(OPC_SIPUSH)
        { NEXT(labels, pc); }
        IMPL(OPC_LDC)
        { NEXT(labels, pc); }
        IMPL(OPC_LDC_W)
        { NEXT(labels, pc); }
        IMPL(OPC_LDC2_W)
        { NEXT(labels, pc); }
        IMPL(OPC_ILOAD)
        { NEXT(labels, pc); }
        IMPL(OPC_LLOAD)
        { NEXT(labels, pc); }
        IMPL(OPC_FLOAD)
        { NEXT(labels, pc); }
        IMPL(OPC_DLOAD)
        { NEXT(labels, pc); }
        IMPL(OPC_ALOAD)
        { NEXT(labels, pc); }
        IMPL(OPC_ILOAD_0)
        { NEXT(labels, pc); }
        IMPL(OPC_ILOAD_1)
        { NEXT(labels, pc); }
        IMPL(OPC_ILOAD_2)
        { NEXT(labels, pc); }
        IMPL(OPC_ILOAD_3)
        { NEXT(labels, pc); }
        IMPL(OPC_LLOAD_0)
        { NEXT(labels, pc); }
        IMPL(OPC_LLOAD_1)
        { NEXT(labels, pc); }
        IMPL(OPC_LLOAD_2)
        { NEXT(labels, pc); }
        IMPL(OPC_LLOAD_3)
        { NEXT(labels, pc); }
        IMPL(OPC_FLOAD_0)
        { NEXT(labels, pc); }
        IMPL(OPC_FLOAD_1)
        { NEXT(labels, pc); }
        IMPL(OPC_FLOAD_2)
        { NEXT(labels, pc); }
        IMPL(OPC_FLOAD_3)
        { NEXT(labels, pc); }
        IMPL(OPC_DLOAD_0)
        { NEXT(labels, pc); }
        IMPL(OPC_DLOAD_1)
        { NEXT(labels, pc); }
        IMPL(OPC_DLOAD_2)
        { NEXT(labels, pc); }
        IMPL(OPC_DLOAD_3)
        { NEXT(labels, pc); }
        IMPL(OPC_ALOAD_0)
        { NEXT(labels, pc); }
        IMPL(OPC_ALOAD_1)
        { NEXT(labels, pc); }
        IMPL(OPC_ALOAD_2)
        { NEXT(labels, pc); }
        IMPL(OPC_ALOAD_3)
        { NEXT(labels, pc); }
        IMPL(OPC_IALOAD)
        { NEXT(labels, pc); }
        IMPL(OPC_LALOAD)
        { NEXT(labels, pc); }
        IMPL(OPC_FALOAD)
        { NEXT(labels, pc); }
        IMPL(OPC_DALOAD)
        { NEXT(labels, pc); }
        IMPL(OPC_AALOAD)
        { NEXT(labels, pc); }
        IMPL(OPC_BALOAD)
        { NEXT(labels, pc); }
        IMPL(OPC_CALOAD)
        { NEXT(labels, pc); }
        IMPL(OPC_SALOAD)
        { NEXT(labels, pc); }
        IMPL(OPC_ISTORE)
        { NEXT(labels, pc); }
        IMPL(OPC_LSTORE)
        { NEXT(labels, pc); }
        IMPL(OPC_FSTORE)
        { NEXT(labels, pc); }
        IMPL(OPC_DSTORE)
        { NEXT(labels, pc); }
        IMPL(OPC_ASTORE)
        { NEXT(labels, pc); }
        IMPL(OPC_ISTORE_0)
        { NEXT(labels, pc); }
        IMPL(OPC_ISTORE_1)
        { NEXT(labels, pc); }
        IMPL(OPC_ISTORE_2)
        { NEXT(labels, pc); }
        IMPL(OPC_ISTORE_3)
        { NEXT(labels, pc); }
        IMPL(OPC_LSTORE_0)
        { NEXT(labels, pc); }
        IMPL(OPC_LSTORE_1)
        { NEXT(labels, pc); }
        IMPL(OPC_LSTORE_2)
        { NEXT(labels, pc); }
        IMPL(OPC_LSTORE_3)
        { NEXT(labels, pc); }
        IMPL(OPC_FSTORE_0)
        { NEXT(labels, pc); }
        IMPL(OPC_FSTORE_1)
        { NEXT(labels, pc); }
        IMPL(OPC_FSTORE_2)
        { NEXT(labels, pc); }
        IMPL(OPC_FSTORE_3)
        { NEXT(labels, pc); }
        IMPL(OPC_DSTORE_0)
        { NEXT(labels, pc); }
        IMPL(OPC_DSTORE_1)
        { NEXT(labels, pc); }
        IMPL(OPC_DSTORE_2)
        { NEXT(labels, pc); }
        IMPL(OPC_DSTORE_3)
        { NEXT(labels, pc); }
        IMPL(OPC_ASTORE_0)
        { NEXT(labels, pc); }
        IMPL(OPC_ASTORE_1)
        { NEXT(labels, pc); }
        IMPL(OPC_ASTORE_2)
        { NEXT(labels, pc); }
        IMPL(OPC_ASTORE_3)
        { NEXT(labels, pc); }
        IMPL(OPC_IASTORE)
        { NEXT(labels, pc); }
        IMPL(OPC_LASTORE)
        { NEXT(labels, pc); }
        IMPL(OPC_FASTORE)
        { NEXT(labels, pc); }
        IMPL(OPC_DASTORE)
        { NEXT(labels, pc); }
        IMPL(OPC_AASTORE)
        { NEXT(labels, pc); }
        IMPL(OPC_BASTORE)
        { NEXT(labels, pc); }
        IMPL(OPC_CASTORE)
        { NEXT(labels, pc); }
        IMPL(OPC_SASTORE)
        { NEXT(labels, pc); }
        IMPL(OPC_POP)
        { NEXT(labels, pc); }
        IMPL(OPC_POP2)
        { NEXT(labels, pc); }
        IMPL(OPC_DUP)
        { NEXT(labels, pc); }
        IMPL(OPC_DUP_X1)
        { NEXT(labels, pc); }
        IMPL(OPC_DUP_X2)
        { NEXT(labels, pc); }
        IMPL(OPC_DUP2)
        { NEXT(labels, pc); }
        IMPL(OPC_DUP2_X1)
        { NEXT(labels, pc); }
        IMPL(OPC_DUP2_X2)
        { NEXT(labels, pc); }
        IMPL(OPC_SWAP)
        { NEXT(labels, pc); }
        IMPL(OPC_IADD)
        { NEXT(labels, pc); }
        IMPL(OPC_LADD)
        { NEXT(labels, pc); }
        IMPL(OPC_FADD)
        { NEXT(labels, pc); }
        IMPL(OPC_DADD)
        { NEXT(labels, pc); }
        IMPL(OPC_ISUB)
        { NEXT(labels, pc); }
        IMPL(OPC_LSUB)
        { NEXT(labels, pc); }
        IMPL(OPC_FSUB)
        { NEXT(labels, pc); }
        IMPL(OPC_DSUB)
        { NEXT(labels, pc); }
        IMPL(OPC_IMUL)
        { NEXT(labels, pc); }
        IMPL(OPC_LMUL)
        { NEXT(labels, pc); }
        IMPL(OPC_FMUL)
        { NEXT(labels, pc); }
        IMPL(OPC_DMUL)
        { NEXT(labels, pc); }
        IMPL(OPC_IDIV)
        { NEXT(labels, pc); }
        IMPL(OPC_LDIV)
        { NEXT(labels, pc); }
        IMPL(OPC_FDIV)
        { NEXT(labels, pc); }
        IMPL(OPC_DDIV)
        { NEXT(labels, pc); }
        IMPL(OPC_IREM)
        { NEXT(labels, pc); }
        IMPL(OPC_LREM)
        { NEXT(labels, pc); }
        IMPL(OPC_FREM)
        { NEXT(labels, pc); }
        IMPL(OPC_DREM)
        { NEXT(labels, pc); }
        IMPL(OPC_INEG)
        { NEXT(labels, pc); }
        IMPL(OPC_LNEG)
        { NEXT(labels, pc); }
        IMPL(OPC_FNEG)
        { NEXT(labels, pc); }
        IMPL(OPC_DNEG)
        { NEXT(labels, pc); }
        IMPL(OPC_ISHL)
        { NEXT(labels, pc); }
        IMPL(OPC_LSHL)
        { NEXT(labels, pc); }
        IMPL(OPC_ISHR)
        { NEXT(labels, pc); }
        IMPL(OPC_LSHR)
        { NEXT(labels, pc); }
        IMPL(OPC_IUSHR)
        { NEXT(labels, pc); }
        IMPL(OPC_LUSHR)
        { NEXT(labels, pc); }
        IMPL(OPC_IAND)
        { NEXT(labels, pc); }
        IMPL(OPC_LAND)
        { NEXT(labels, pc); }
        IMPL(OPC_IOR)
        { NEXT(labels, pc); }
        IMPL(OPC_LOR)
        { NEXT(labels, pc); }
        IMPL(OPC_IXOR)
        { NEXT(labels, pc); }
        IMPL(OPC_LXOR)
        { NEXT(labels, pc); }
        IMPL(OPC_IINC)
        { NEXT(labels, pc); }
        IMPL(OPC_I2L)
        { NEXT(labels, pc); }
        IMPL(OPC_I2F)
        { NEXT(labels, pc); }
        IMPL(OPC_I2D)
        { NEXT(labels, pc); }
        IMPL(OPC_L2I)
        { NEXT(labels, pc); }
        IMPL(OPC_L2F)
        { NEXT(labels, pc); }
        IMPL(OPC_L2D)
        { NEXT(labels, pc); }
        IMPL(OPC_F2I)
        { NEXT(labels, pc); }
        IMPL(OPC_F2L)
        { NEXT(labels, pc); }
        IMPL(OPC_F2D)
        { NEXT(labels, pc); }
        IMPL(OPC_D2I)
        { NEXT(labels, pc); }
        IMPL(OPC_D2L)
        { NEXT(labels, pc); }
        IMPL(OPC_D2F)
        { NEXT(labels, pc); }
        IMPL(OPC_I2B)
        { NEXT(labels, pc); }
        IMPL(OPC_I2C)
        { NEXT(labels, pc); }
        IMPL(OPC_I2S)
        { NEXT(labels, pc); }
        IMPL(OPC_LCMP)
        { NEXT(labels, pc); }
        IMPL(OPC_FCMPL)
        { NEXT(labels, pc); }
        IMPL(OPC_FCMPG)
        { NEXT(labels, pc); }
        IMPL(OPC_DCMPL)
        { NEXT(labels, pc); }
        IMPL(OPC_DCMPG)
        { NEXT(labels, pc); }
        IMPL(OPC_IFEQ)
        { NEXT(labels, pc); }
        IMPL(OPC_IFNE)
        { NEXT(labels, pc); }
        IMPL(OPC_IFLT)
        { NEXT(labels, pc); }
        IMPL(OPC_IFGE)
        { NEXT(labels, pc); }
        IMPL(OPC_IFGT)
        { NEXT(labels, pc); }
        IMPL(OPC_IFLE)
        { NEXT(labels, pc); }
        IMPL(OPC_IF_ICMPEQ)
        { NEXT(labels, pc); }
        IMPL(OPC_IF_ICMPNE)
        { NEXT(labels, pc); }
        IMPL(OPC_IF_ICMPLT)
        { NEXT(labels, pc); }
        IMPL(OPC_IF_ICMPGE)
        { NEXT(labels, pc); }
        IMPL(OPC_IF_ICMPGT)
        { NEXT(labels, pc); }
        IMPL(OPC_IF_ICMPLE)
        { NEXT(labels, pc); }
        IMPL(OPC_IF_ACMPEQ)
        { NEXT(labels, pc); }
        IMPL(OPC_IF_ACMPNE)
        { NEXT(labels, pc); }
        IMPL(OPC_GOTO)
        { NEXT(labels, pc); }
        IMPL(OPC_JSR)
        { NEXT(labels, pc); }
        IMPL(OPC_RET)
        { NEXT(labels, pc); }
        IMPL(OPC_TABLESWITCH)
        { NEXT(labels, pc); }
        IMPL(OPC_LOOKUPSWITCH)
        { NEXT(labels, pc); }
        IMPL(OPC_IRETURN)
        { NEXT(labels, pc); }
        IMPL(OPC_LRETURN)
        { NEXT(labels, pc); }
        IMPL(OPC_FRETURN)
        { NEXT(labels, pc); }
        IMPL(OPC_DRETURN)
        { NEXT(labels, pc); }
        IMPL(OPC_ARETURN)
        { NEXT(labels, pc); }
        IMPL(OPC_RETURN)
        { NEXT(labels, pc); }
        IMPL(OPC_GETSTATIC)
        { NEXT(labels, pc); }
        IMPL(OPC_PUTSTATIC)
        { NEXT(labels, pc); }
        IMPL(OPC_GETFIELD)
        { NEXT(labels, pc); }
        IMPL(OPC_PUTFIELD)
        { NEXT(labels, pc); }
        IMPL(OPC_INVOKEVIRTUAL)
        { NEXT(labels, pc); }
        IMPL(OPC_INVOKESPECIAL)
        { NEXT(labels, pc); }
        IMPL(OPC_INVOKESTATIC)
        { NEXT(labels, pc); }
        IMPL(OPC_INVOKEINTERFACE)
        { NEXT(labels, pc); }
        IMPL(OPC_INVOKEDYNAMIC)
        { NEXT(labels, pc); }
        IMPL(OPC_NEW)
        { NEXT(labels, pc); }
        IMPL(OPC_NEWARRAY)
        { NEXT(labels, pc); }
        IMPL(OPC_ANEWARRAY)
        { NEXT(labels, pc); }
        IMPL(OPC_ARRAYLENGTH)
        { NEXT(labels, pc); }
        IMPL(OPC_ATHROW)
        { NEXT(labels, pc); }
        IMPL(OPC_CHECKCAST)
        { NEXT(labels, pc); }
        IMPL(OPC_INSTANCEOF)
        { NEXT(labels, pc); }
        IMPL(OPC_MONITORENTER)
        { NEXT(labels, pc); }
        IMPL(OPC_MONITOREXIT)
        { NEXT(labels, pc); }
        IMPL(OPC_WIDE)
        { NEXT(labels, pc); }
        IMPL(OPC_MULTIANEWARRAY)
        { NEXT(labels, pc); }
        IMPL(OPC_IFNULL)
        { NEXT(labels, pc); }
        IMPL(OPC_IFNONNULL)
        { NEXT(labels, pc); }
        IMPL(OPC_GOTO_W)
        { NEXT(labels, pc); }
        IMPL(OPC_JSR_W)
        { NEXT(labels, pc); }
        IMPL(OPC_LDC_QUICK)
        { NEXT(labels, pc); }
        IMPL(OPC_LDC_W_QUICK)
        { NEXT(labels, pc); }
        IMPL(OPC_GETFIELD_QUICK)
        { NEXT(labels, pc); }
        IMPL(OPC_PUTFIELD_QUICK)
        { NEXT(labels, pc); }
        IMPL(OPC_GETFIELD2_QUICK)
        { NEXT(labels, pc); }
        IMPL(OPC_PUTFIELD2_QUICK)
        { NEXT(labels, pc); }
        IMPL(OPC_GETSTATIC_QUICK)
        { NEXT(labels, pc); }
        IMPL(OPC_PUTSTATIC_QUICK)
        { NEXT(labels, pc); }
        IMPL(OPC_GETSTATIC2_QUICK)
        { NEXT(labels, pc); }
        IMPL(OPC_PUTSTATIC2_QUICK)
        { NEXT(labels, pc); }
        IMPL(OPC_INVOKEVIRTUAL_QUICK)
        { NEXT(labels, pc); }
        IMPL(OPC_INVOKENONVIRTUAL_QUICK)
        { NEXT(labels, pc); }
        IMPL(OPC_INVOKESUPER_QUICK)
        { NEXT(labels, pc); }
        IMPL(OPC_GETFIELD_QUICK_REF)
        { NEXT(labels, pc); }
        IMPL(OPC_PUTFIELD_QUICK_REF)
        { NEXT(labels, pc); }
        IMPL(OPC_GETSTATIC_QUICK_REF)
        { NEXT(labels, pc); }
        IMPL(OPC_PUTSTATIC_QUICK_REF)
        { NEXT(labels, pc); }
        IMPL(OPC_GETFIELD_THIS_REF)
        { NEXT(labels, pc); }
        IMPL(OPC_MIRANDA_BRIDGE)
        { NEXT(labels, pc); }
        IMPL(OPC_ABSTRACT_METHOD_ERROR)
        { NEXT(labels, pc); }
        IMPL(OPC_INLINE_REWRITER)
        { NEXT(labels, pc); }
        IMPL(OPC_PROFILE_REWRITER)
        { NEXT(labels, pc); }
        IMPL(OPC_INVOKEVIRTUAL_QUICK_W)
        { NEXT(labels, pc); }
        IMPL(OPC_GETFIELD_QUICK_W)
        { NEXT(labels, pc); }
        IMPL(OPC_PUTFIELD_QUICK_W)
        { NEXT(labels, pc); }
        IMPL(OPC_GETFIELD_THIS)
        { NEXT(labels, pc); }
        IMPL(OPC_LOCK)
        { NEXT(labels, pc); }
        IMPL(OPC_ALOAD_THIS)
        { NEXT(labels, pc); }
        IMPL(OPC_INVOKESTATIC_QUICK)
        { NEXT(labels, pc); }
        IMPL(OPC_NEW_QUICK)
        { NEXT(labels, pc); }
        IMPL(OPC_ANEWARRAY_QUICK)
        { NEXT(labels, pc); }
        IMPL(OPC_CHECKCAST_QUICK)
        { NEXT(labels, pc); }
        IMPL(OPC_INSTANCEOF_QUICK)
        { NEXT(labels, pc); }
        IMPL(OPC_MULTIANEWARRAY_QUICK)
        { NEXT(labels, pc); }
        IMPL(OPC_INVOKEHANDLE)
        { NEXT(labels, pc); }
        IMPL(OPC_INVOKEBASIC)
        { NEXT(labels, pc); }
        IMPL(OPC_LINKTOSPECIAL)
        { NEXT(labels, pc); }
        IMPL(OPC_LINKTOVIRTUAL)
        { NEXT(labels, pc); }
        IMPL(OPC_LINKTOINTERFACE)
        { NEXT(labels, pc); }
        IMPL(OPC_INVOKEINTERFACE_QUICK)
        { NEXT(labels, pc); }
        IMPL(OPC_INVOKEDYNAMIC_QUICK)
        { NEXT(labels, pc); }

        OPCODE_END()
        return nullptr;
    }
}
