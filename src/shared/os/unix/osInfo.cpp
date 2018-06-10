//
// Created by kiva on 2018/6/10.
//

#include <shared/osInfo.h>
#include <sys/utsname.h>
#include <unistd.h>

#if defined(KIVM_PLATFORM_UNIX)
namespace kivm {
    String UnixInformation::getOSName() {
        struct utsname name{};
        uname(&name);
        return strings::fromStdString(name.sysname);
    }

    String UnixInformation::getOSVersion() {
        struct utsname name{};
        uname(&name);
        return strings::fromStdString(name.release);
    }

    int UnixInformation::getCpuNumbers() {
        static long ncpu = sysconf(_SC_NPROCESSORS_CONF);
        return (int) ncpu;
    }
}
#endif