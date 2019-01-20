//
// Created by kiva on 2018/6/10.
//
#include <shared/osInfo.h>

#if defined(KIVM_PLATFORM_WINDOWS)

namespace kivm {
    String WindowsInformation::getOSName() {
        return L"Windows";
    }

    String WindowsInformation::getOSVersion() {
        return L"unknown";
    }

    int WindowsInformation::getCpuNumbers() {
        return 1;
    }
}
#endif
