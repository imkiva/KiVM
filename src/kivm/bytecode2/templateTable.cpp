//
// Created by kiva on 2019-06-28.
//
#include <kivm/bytecode2/templateTable.h>

namespace kivm {
    bool TemplateTable::_is_initialized = false;
    Template TemplateTable::_templateTable[Bytecodes::number_of_codes];
    Template TemplateTable::_templateTableWide[Bytecodes::number_of_codes];
    Template *TemplateTable::_desc = nullptr;
    InterpreterMacroAssembler *TemplateTable::_masm = nullptr;


    void TemplateTable::initialize() {
        // TODO
    }
}