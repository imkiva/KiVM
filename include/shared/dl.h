//
// Created by kiva on 2018/4/15.
//

#pragma once

#include <shared/os/unix/dl.h>
#include <shared/os/windows/dl.h>

namespace kivm {
    namespace dl {
#if defined(KIVM_PLATFORM_UNIX) || defined(KIVM_PLATFORM_APPLE)
        using DLInterface = UnixDLInterface;
#elif KIVM_PLATFORM_WINDOWS
        using DLInterface = WindowsDLInterface;
#endif
    }
}
