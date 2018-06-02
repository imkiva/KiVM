//
// Created by kiva on 2018/6/3.
//
#pragma once

#include <compileTimeConfig.h>

#if KIVM_THREADED

#include <kivm/bytecode/threadedInterpreter.h>

namespace kivm {
    using DefaultInterpreter = ThreadedInterpreter;
}

#else

#include <kivm/bytecode/bytecodeInterpreter.h>

namespace kivm {
    using DefaultInterpreter = ByteCodeInterpreter;
}


#endif
