//
// Created by kiva on 2019-06-28.
//
#include <kivm/bytecode2/template.h>
#include <kivm/bytecode2/templateTable.h>

namespace kivm {
    void Template::initialize(int flags, TosState tosIn, TosState tosOut, Template::generator gen, int arg) {
        _flags = flags;
        _tosIn = tosIn;
        _tosOut = tosOut;
        _gen = gen;
        _arg = arg;
    }

    Bytecodes::Code Template::getBytecode() const {
        int i = this - TemplateTable::_templateTable;
        if (i < 0 || i >= Bytecodes::number_of_codes) i = this - TemplateTable::_templateTableWide;
        return static_cast<Bytecodes::Code>(i);
    }

    void Template::generate(InterpreterMacroAssembler *masm) {
        // parameter passing
        TemplateTable::_desc = this;
        TemplateTable::_masm = masm;
        // code generation
        _gen(_arg);
        masm->flush();
    }
}
