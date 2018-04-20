//
// Created by kiva on 2018/3/25.
//
#pragma once

#include <shared/types.h>

namespace kivm {
    struct RuntimeConfig {
        int threadMaxStackFrames;
        size_t initialHeapSizeInBytes;
        size_t maxHeapSizeInBytes;

        static RuntimeConfig &get();

        RuntimeConfig();
    };
}

