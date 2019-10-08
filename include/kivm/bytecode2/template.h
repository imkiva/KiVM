//
// Created by kiva on 2019-06-28.
//
#pragma once

#include <kivm/kivm.h>
#include <kivm/bytecode2/defs.h>
#include <kivm/bytecode2/bytecodes.h>
#include <kivm/bytecode2/interpreterMacroAssembler.h>

namespace kivm {
    class TemplateTable;

    // A Template describes the properties of a code template for a given bytecode
    // and provides a generator to generate the code template.
    class Template {
        friend class TemplateTable;

    private:
        enum Flags {
            usesBcpBit,                   // set if template needs the bcp pointing to bytecode
            doesDispatchBit,              // set if template dispatches on its own
            callsVMBit,                   // set if template calls the vm
            wideBit                       // set if template belongs to a wide instruction
        };

        typedef void (*generator)(int arg);

        int _flags;                       // describes interpreter template properties (bcp unknown)
        TosState _tosIn;                  // tos cache state before template execution
        TosState _tosOut;                 // tos cache state after  template execution
        generator _gen;                   // template code generator
        int _arg;                         // argument for template code generator

        void initialize(int flags, TosState tosIn, TosState tosOut, generator gen, int arg);

    public:
        Bytecodes::Code getBytecode() const;

        bool isValid() const { return _gen != nullptr; }

        bool usesBcp() const { return (_flags & (1 << usesBcpBit)) != 0; }

        bool doesDispatch() const { return (_flags & (1 << doesDispatchBit)) != 0; }

        bool callsVM() const { return (_flags & (1 << callsVMBit)) != 0; }

        bool isWide() const { return (_flags & (1 << wideBit)) != 0; }

        TosState tosIn() const { return _tosIn; }

        TosState tosOut() const { return _tosOut; }

        void generate(InterpreterMacroAssembler *masm);
    };
}
