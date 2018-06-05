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
        threadMaxStackFrames = 512;
        initialHeapSizeInBytes = SIZE_MB(64);
        maxHeapSizeInBytes = SIZE_MB(512);
    }
}
