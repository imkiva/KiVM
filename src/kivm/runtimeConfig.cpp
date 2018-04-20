//
// Created by kiva on 2018/3/25.
//
#include <kivm/runtime/runtimeConfig.h>

#define BYTES_UNIT 1024
#define SIZE_KB(X) ((X) * BYTES_UNIT)
#define SIZE_MB(X) ((X) * SIZE_KB(BYTES_UNIT))

namespace kivm {
    RuntimeConfig &RuntimeConfig::get() {
        static RuntimeConfig RUNTIME_CONFIG;
        return RUNTIME_CONFIG;
    }

    RuntimeConfig::RuntimeConfig() {
        threadMaxStackFrames = 512;
        initialHeapSizeInBytes = SIZE_MB(256);
        maxHeapSizeInBytes = SIZE_MB(512);
    }
}
