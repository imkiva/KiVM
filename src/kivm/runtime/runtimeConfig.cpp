//
// Created by kiva on 2018/3/25.
//
#include <kivm/runtime/runtimeConfig.h>

namespace kivm {
    RuntimeConfig &RuntimeConfig::get() {
        static RuntimeConfig RUNTIME_CONFIG;
        return RUNTIME_CONFIG;
    }

    RuntimeConfig::RuntimeConfig() {
        threadMaxStackFrames = 1024;
        initialHeapSizeInBytes = SIZE_MB(512L);
        maxHeapSizeInBytes = SIZE_MB(2048L);
    }
}
