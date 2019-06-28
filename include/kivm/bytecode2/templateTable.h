//
// Created by kiva on 2019-06-28.
//
#pragma once

#include <kivm/bytecode2/template.h>
#include <kivm/asm/address.h>
#include <kivm/asm/register.h>

namespace kivm {
    class TemplateTable {
    public:
        enum Operation {
            add, sub, mul, div, rem, _and, _or, _xor, shl, shr, ushr
        };
        enum Condition {
            equal, not_equal, less, less_equal, greater, greater_equal
        };
        enum CacheByte {
            f1_byte = 1, f2_byte = 2
        };  // byteNo codes

    private:
        // true if TemplateTable has been initialized
        static bool _is_initialized;

        static Template _templateTable[Bytecodes::number_of_codes];
        static Template _templateTableWide[Bytecodes::number_of_codes];

        // the current template to be generated
        static Template *_desc;

        static Bytecodes::Code getBytecode() { return _desc->getBytecode(); }

    public:
        // the assembler used when generating templates
        static InterpreterMacroAssembler *_masm;

    private:

        // special registers
        static inline Address atBcp(int offset);

        // helpers
        static void unimplementedBytecode();

        // C calls
        static void call_VM(Register oopResult, address entryPoint);

        static void call_VM(Register oopResult, address entryPoint, Register arg_1);

        static void call_VM(Register oopResult, address entryPoint, Register arg_1, Register arg_2);

        static void call_VM(Register oopResult, address entryPoint, Register arg_1, Register arg_2, Register arg_3);

        static void call_VM(Register oopResult, Register lastJavaSp, address entryPoint);

        static void call_VM(Register oopResult, Register lastJavaSp, address entryPoint, Register arg_1);

        static void
        call_VM(Register oopResult, Register lastJavaSp, address entryPoint, Register arg_1, Register arg_2);

        static void call_VM(Register oopResult, Register lastJavaSp, address entryPoint, Register arg_1, Register arg_2,
                            Register arg_3);

        // bytecodes
        static void nop();

        static void aconst_null();

        static void iconst(int value);

        static void lconst(int value);

        static void fconst(int value);

        static void dconst(int value);

        static void bipush();

        static void sipush();

        static void ldc(bool wide);

        static void ldc2_w();

        static void fast_aldc(bool wide);

        static void locals_index(Register reg, int offset = 1);

        static void iload();

        static void fast_iload();

        static void fast_iload2();

        static void fast_icaload();

        static void lload();

        static void fload();

        static void dload();

        static void aload();

        static void locals_index_wide(Register reg);

        static void wide_iload();

        static void wide_lload();

        static void wide_fload();

        static void wide_dload();

        static void wide_aload();

        static void iaload();

        static void laload();

        static void faload();

        static void daload();

        static void aaload();

        static void baload();

        static void caload();

        static void saload();

        static void iload(int n);

        static void lload(int n);

        static void fload(int n);

        static void dload(int n);

        static void aload(int n);

        static void aload_0();

        static void istore();

        static void lstore();

        static void fstore();

        static void dstore();

        static void astore();

        static void wide_istore();

        static void wide_lstore();

        static void wide_fstore();

        static void wide_dstore();

        static void wide_astore();

        static void iastore();

        static void lastore();

        static void fastore();

        static void dastore();

        static void aastore();

        static void bastore();

        static void castore();

        static void sastore();

        static void istore(int n);

        static void lstore(int n);

        static void fstore(int n);

        static void dstore(int n);

        static void astore(int n);

        static void pop();

        static void pop2();

        static void dup();

        static void dup_x1();

        static void dup_x2();

        static void dup2();

        static void dup2_x1();

        static void dup2_x2();

        static void swap();

        static void iop2(Operation op);

        static void lop2(Operation op);

        static void fop2(Operation op);

        static void dop2(Operation op);

        static void idiv();

        static void irem();

        static void lmul();

        static void ldiv();

        static void lrem();

        static void lshl();

        static void lshr();

        static void lushr();

        static void ineg();

        static void lneg();

        static void fneg();

        static void dneg();

        static void iinc();

        static void wide_iinc();

        static void convert();

        static void lcmp();

        static void float_cmp(bool isFloat, int unorderedResult);

        static void float_cmp(int unorderedResult);

        static void double_cmp(int unorderedResult);

        static void count_calls(Register method, Register temp);

        static void branch(bool is_jsr, bool is_wide);

        static void if_0cmp(Condition cc);

        static void if_icmp(Condition cc);

        static void if_nullcmp(Condition cc);

        static void if_acmp(Condition cc);

        static void _goto();

        static void jsr();

        static void ret();

        static void wide_ret();

        static void goto_w();

        static void jsr_w();

        static void tableswitch();

        static void lookupswitch();

        static void fast_linearswitch();

        static void fast_binaryswitch();

        static void _return(TosState state);

        static void resolve_cache_and_index(int byteNo,       // one of 1,2,11
                                            Register cache,    // output for CP cache
                                            Register index,    // output for CP index
                                            size_t indexSize); // one of 1,2,4

        static void load_invoke_cp_cache_entry(int byteNo,
                                               Register method,
                                               Register itableIndex,
                                               Register flags,
                                               bool is_invokevirtual,
                                               bool is_virtual_final,
                                               bool is_invokedynamic);

        static void load_field_cp_cache_entry(Register obj,
                                              Register cache,
                                              Register index,
                                              Register offset,
                                              Register flags,
                                              bool isStatic);

        static void invokevirtual(int byteNo);

        static void invokespecial(int byteNo);

        static void invokestatic(int byteNo);

        static void invokeinterface(int byteNo);

        static void invokedynamic(int byteNo);

        static void invokehandle(int byteNo);

        static void fast_invokevfinal(int byteNo);

        static void getfield_or_static(int byteNo, bool isStatic);

        static void putfield_or_static(int byteNo, bool isStatic);

        static void getfield(int byteNo);

        static void putfield(int byteNo);

        static void getstatic(int byteNo);

        static void putstatic(int byteNo);

        static void pop_and_check_object(Register obj);

        static void _new();

        static void newarray();

        static void anewarray();

        static void arraylength();

        static void checkcast();

        static void instanceof();

        static void athrow();

        static void monitorenter();

        static void monitorexit();

        static void wide();

        static void multianewarray();

        static void _breakpoint();

        static void shouldnotreachhere();

        // debugging of TemplateGenerator
        // checks if in/out states expected by template generator correspond to table entries
        static void transition(TosState in, TosState out);

        // initialization helpers
        static void def(Bytecodes::Code code, int flags, TosState in, TosState out, void (*gen)(), char filler);

        static void def(Bytecodes::Code code, int flags, TosState in, TosState out, void (*gen)(int arg), int arg);

        static void def(Bytecodes::Code code, int flags, TosState in, TosState out, void (*gen)(bool arg), bool arg);

        static void
        def(Bytecodes::Code code, int flags, TosState in, TosState out, void (*gen)(TosState tos), TosState tos);

        static void
        def(Bytecodes::Code code, int flags, TosState in, TosState out, void (*gen)(Operation op), Operation op);

        static void
        def(Bytecodes::Code code, int flags, TosState in, TosState out, void (*gen)(Condition cc), Condition cc);

        friend class Template;

        // InterpreterMacroAssembler::is_a(), etc., need TemplateTable::call_VM().
        friend class InterpreterMacroAssembler;

    public:
        // Initialization
        static void initialize();

        // Templates
        static Template *templateFor(Bytecodes::Code code) {
            Bytecodes::check(code);
            return &_templateTable[code];
        }

        static Template *templateForWide(Bytecodes::Code code) {
            Bytecodes::wideCheck(code);
            return &_templateTableWide[code];
        }

        // Platform specifics comes here
    };
}
