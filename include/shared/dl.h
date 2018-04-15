//
// Created by kiva on 2018/4/15.
//

#pragma once

#if defined(KIVM_PLATFORM_UNIX)

#include <shared/platform/unix/dl.h>

#elif defined(KIVM_PLATFORM_WINDOWS)

#include <shared/platform/windows/dl.hpp>

#endif

namespace kivm {
    namespace dl {
#ifdef KIVM_PLATFORM_UNIX
        using DLInterface = UnixDLInterface;
#elif KIVM_PLATFORM_WINDOWS
        using DLInterface = WindowsDLInterface;
#endif
    }
}
