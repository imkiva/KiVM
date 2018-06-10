//
// Created by kiva on 2018/6/10.
//

#pragma once

#include <compileTimeConfig.h>
#include <shared/string.h>

#if defined(KIVM_PLATFORM_UNIX)
namespace kivm {
    class UnixInformation {
    public:
        static String getOSName();

        static String getOSVersion();

        static int getCpuNumbers();
    };
}
#endif
