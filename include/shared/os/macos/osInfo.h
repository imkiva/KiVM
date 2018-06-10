//
// Created by kiva on 2018/6/10.
//

#pragma once

#include <compileTimeConfig.h>
#include <shared/string.h>

#if defined(KIVM_PLATFORM_APPLE)
namespace kivm {
    class MacOSInformation {
    public:
        static String getOSName();

        static String getOSVersion();
    };
}
#endif
