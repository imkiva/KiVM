//
// Created by kiva on 2018/4/21.
//
#include <shared/os/unix/filesystem.h>
#include <shared/os/windows/filesystem.h>

namespace kivm {
#if defined(KIVM_PLATFORM_UNIX) || defined(KIVM_PLATFORM_APPLE)
    using FileSystem = UnixFileSystem;
#elif defined(KIVM_PLATFORM_WINDOWS)
    using FileSystem = WindowsFileSystem;
#else
#error Unknow platform
#endif
}
