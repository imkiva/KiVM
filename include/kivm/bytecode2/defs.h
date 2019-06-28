//
// Created by kiva on 2019-06-22.
//
#pragma once

#include <kivm/bytecode/bytecodes.h>
#include <cstdint>

#define INSTR_NOP                   OPC_NOP
#define INSTR_ICONST                OPC_ICONST_0
#define INSTR_LCONST                OPC_LCONST_0
#define INSTR_FCONST                OPC_FCONST_0
#define INSTR_DCONST                OPC_DCONST_0
#define INSTR_BIPUSH                OPC_BIPUSH
#define INSTR_SIPUSH                OPC_SIPUSH
#define INSTR_LDC                   OPC_LDC
#define INSTR_LDC_W                 OPC_LDC_W
#define INSTR_LDC2_W                OPC_LDC2_W
#define INSTR_ILOAD                 OPC_ILOAD_0
#define INSTR_LLOAD                 OPC_LLOAD_0
#define INSTR_FLOAD                 OPC_FLOAD_0
#define INSTR_DLOAD                 OPC_DLOAD_0
#define INSTR_ALOAD                 OPC_ALOAD_0
#define INSTR_IALOAD                OPC_IALOAD
#define INSTR_LALOAD                OPC_LALOAD
#define INSTR_FALOAD                OPC_FALOAD
#define INSTR_DALOAD                OPC_DALOAD
#define INSTR_AALOAD                OPC_AALOAD
#define INSTR_BALOAD                OPC_BALOAD
#define INSTR_CALOAD                OPC_CALOAD
#define INSTR_SALOAD                OPC_SALOAD
#define INSTR_ISTORE                OPC_ISTORE_0
#define INSTR_LSTORE                OPC_LSTORE_0
#define INSTR_FSTORE                OPC_FSTORE_0
#define INSTR_DSTORE                OPC_DSTORE_0
#define INSTR_ASTORE                OPC_ASTORE_0
#define INSTR_IASTORE               OPC_IASTORE
#define INSTR_LASTORE               OPC_LASTORE
#define INSTR_FASTORE               OPC_FASTORE
#define INSTR_DASTORE               OPC_DASTORE
#define INSTR_AASTORE               OPC_AASTORE
#define INSTR_BASTORE               OPC_BASTORE
#define INSTR_CASTORE               OPC_CASTORE
#define INSTR_SASTORE               OPC_SASTORE
#define INSTR_POP                   OPC_POP
#define INSTR_POP2                  OPC_POP2
#define INSTR_DUP                   OPC_DUP
#define INSTR_DUP_X1                OPC_DUP_X1
#define INSTR_DUP_X2                OPC_DUP_X2
#define INSTR_DUP2                  OPC_DUP2
#define INSTR_DUP2_X1               OPC_DUP2_X1
#define INSTR_DUP2_X2               OPC_DUP2_X2
#define INSTR_SWAP                  OPC_SWAP
#define INSTR_IADD                  OPC_IADD
#define INSTR_LADD                  OPC_LADD
#define INSTR_FADD                  OPC_FADD
#define INSTR_DADD                  OPC_DADD
#define INSTR_ISUB                  OPC_ISUB
#define INSTR_LSUB                  OPC_LSUB
#define INSTR_FSUB                  OPC_FSUB
#define INSTR_DSUB                  OPC_DSUB
#define INSTR_IMUL                  OPC_IMUL
#define INSTR_LMUL                  OPC_LMUL
#define INSTR_FMUL                  OPC_FMUL
#define INSTR_DMUL                  OPC_DMUL
#define INSTR_IDIV                  OPC_IDIV
#define INSTR_LDIV                  OPC_LDIV
#define INSTR_FDIV                  OPC_FDIV
#define INSTR_DDIV                  OPC_DDIV
#define INSTR_IREM                  OPC_IREM
#define INSTR_LREM                  OPC_LREM
#define INSTR_FREM                  OPC_FREM
#define INSTR_DREM                  OPC_DREM
#define INSTR_INEG                  OPC_INEG
#define INSTR_LNEG                  OPC_LNEG
#define INSTR_FNEG                  OPC_FNEG
#define INSTR_DNEG                  OPC_DNEG
#define INSTR_ISHL                  OPC_ISHL
#define INSTR_LSHL                  OPC_LSHL
#define INSTR_ISHR                  OPC_ISHR
#define INSTR_LSHR                  OPC_LSHR
#define INSTR_IUSHR                 OPC_IUSHR
#define INSTR_LUSHR                 OPC_LUSHR
#define INSTR_IAND                  OPC_IAND
#define INSTR_LAND                  OPC_LAND
#define INSTR_IOR                   OPC_IOR
#define INSTR_LOR                   OPC_LOR
#define INSTR_IXOR                  OPC_IXOR
#define INSTR_LXOR                  OPC_LXOR
#define INSTR_IINC                  OPC_IINC
#define INSTR_I2L                   OPC_I2L
#define INSTR_I2F                   OPC_I2F
#define INSTR_I2D                   OPC_I2D
#define INSTR_L2I                   OPC_L2I
#define INSTR_L2F                   OPC_L2F
#define INSTR_L2D                   OPC_L2D
#define INSTR_F2I                   OPC_F2I
#define INSTR_F2L                   OPC_F2L
#define INSTR_F2D                   OPC_F2D
#define INSTR_D2I                   OPC_D2I
#define INSTR_D2L                   OPC_D2L
#define INSTR_D2F                   OPC_D2F
#define INSTR_I2B                   OPC_I2B
#define INSTR_I2C                   OPC_I2C
#define INSTR_I2S                   OPC_I2S
#define INSTR_LCMP                  OPC_LCMP
#define INSTR_FCMPL                 OPC_FCMPL
#define INSTR_FCMPG                 OPC_FCMPG
#define INSTR_DCMPL                 OPC_DCMPL
#define INSTR_DCMPG                 OPC_DCMPG
#define INSTR_IFEQ                  OPC_IFEQ
#define INSTR_IFNE                  OPC_IFNE
#define INSTR_IFLT                  OPC_IFLT
#define INSTR_IFGE                  OPC_IFGE
#define INSTR_IFGT                  OPC_IFGT
#define INSTR_IFLE                  OPC_IFLE
#define INSTR_IF_ICMPEQ             OPC_IF_ICMPEQ
#define INSTR_IF_ICMPNE             OPC_IF_ICMPNE
#define INSTR_IF_ICMPLT             OPC_IF_ICMPLT
#define INSTR_IF_ICMPGE             OPC_IF_ICMPGE
#define INSTR_IF_ICMPGT             OPC_IF_ICMPGT
#define INSTR_IF_ICMPLE             OPC_IF_ICMPLE
#define INSTR_IF_ACMPEQ             OPC_IF_ACMPEQ
#define INSTR_IF_ACMPNE             OPC_IF_ACMPNE
#define INSTR_GOTO                  OPC_GOTO
#define INSTR_JSR                   OPC_JSR
#define INSTR_RET                   OPC_RET
#define INSTR_TABLESWITCH           OPC_TABLESWITCH
#define INSTR_LOOKUPSWITCH          OPC_LOOKUPSWITCH
#define INSTR_IRETURN               OPC_IRETURN
#define INSTR_LRETURN               OPC_LRETURN
#define INSTR_FRETURN               OPC_FRETURN
#define INSTR_DRETURN               OPC_DRETURN
#define INSTR_ARETURN               OPC_ARETURN
#define INSTR_RETURN                OPC_RETURN
#define INSTR_GETSTATIC             OPC_GETSTATIC
#define INSTR_PUTSTATIC             OPC_PUTSTATIC
#define INSTR_GETFIELD              OPC_GETFIELD
#define INSTR_PUTFIELD              OPC_PUTFIELD
#define INSTR_INVOKEVIRTUAL         OPC_INVOKEVIRTUAL
#define INSTR_INVOKESPECIAL         OPC_INVOKESPECIAL
#define INSTR_INVOKESTATIC          OPC_INVOKESTATIC
#define INSTR_INVOKEINTERFACE       OPC_INVOKEINTERFACE
#define INSTR_INVOKEDYNAMIC         OPC_INVOKEDYNAMIC
#define INSTR_NEW                   OPC_NEW
#define INSTR_NEWARRAY              OPC_NEWARRAY
#define INSTR_ANEWARRAY             OPC_ANEWARRAY
#define INSTR_ARRAYLENGTH           OPC_ARRAYLENGTH
#define INSTR_ATHROW                OPC_ATHROW
#define INSTR_CHECKCAST             OPC_CHECKCAST
#define INSTR_INSTANCEOF            OPC_INSTANCEOF
#define INSTR_MONITORENTER          OPC_MONITORENTER
#define INSTR_MONITOREXIT           OPC_MONITOREXIT
#define INSTR_WIDE                  OPC_WIDE
#define INSTR_MULTIANEWARRAY        OPC_MULTIANEWARRAY
#define INSTR_IFNULL                OPC_IFNULL
#define INSTR_IFNONNULL             OPC_IFNONNULL
#define INSTR_GOTO_W                OPC_GOTO_W
#define INSTR_JSR_W                 OPC_JSR_W
#define INSTR_BREAKPOINT            OPC_BREAKPOINT
#define INSTR_IMPDEP1               OPC_IMPDEP1
#define INSTR_IMPDEP2               OPC_IMPDEP2
#define INSTR_NUM_OPCODES           OPC_NUM_OPCODES

namespace kivm {
    using address = void *;

    enum TosState {           // describes the tos cache contents
        btos = 0,             // byte, bool tos cached
        ztos = 1,             // byte, bool tos cached
        ctos = 2,             // char tos cached
        stos = 3,             // short tos cached
        itos = 4,             // int tos cached
        ltos = 5,             // long tos cached
        ftos = 6,             // float tos cached
        dtos = 7,             // double tos cached
        atos = 8,             // object cached
        vtos = 9,             // tos not cached
        number_of_states,
        ilgl                  // illegal state: should not occur
    };
}