//
// Created by kiva on 2018/6/10.
//
#pragma once

#include <shared/os/unix/osInfo.h>
#include <shared/os/macos/osInfo.h>
#include <shared/os/windows/osInfo.h>

namespace kivm {
#if defined(KIVM_PLATFORM_APPLE)
    using OSInformation = MacOSInformation;
#elif defined(KIVM_PLATFORM_UNIX)
    using OSInformation = UnixInformation;
#elif defined(KIVM_PLATFORM_WINDOWS)
    using OSInformation = WindowsInformation;
#else
#error Unknow platform
#endif
}
