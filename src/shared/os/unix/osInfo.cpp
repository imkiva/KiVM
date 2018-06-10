//
// Created by kiva on 2018/6/10.
//

#include <shared/osInfo.h>
#include <sys/utsname.h>

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
}
#endif