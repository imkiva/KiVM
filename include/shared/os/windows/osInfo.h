//
// Created by kiva on 2018/6/10.
//

#pragma once

#include <compileTimeConfig.h>

#ifdef KIVM_PLATFORM_WINDOWS
namespace kivm {
    class WindowsInformation final {
    public:
        static String getOSName();

        static String getOSVersion();

        static int getCpuNumbers();
    };
}
#endif
