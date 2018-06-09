//
// Created by kiva on 2018/6/3.
//
#pragma once

#include <compileTimeConfig.h>

#if defined(KIVM_THREADED) && !defined(KIVM_DEBUG)

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
