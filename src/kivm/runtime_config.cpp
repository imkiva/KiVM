//
// Created by kiva on 2018/3/25.
//
#include <kivm/runtime_config.h>

namespace kivm {
    RuntimeConfig &RuntimeConfig::get() {
        static RuntimeConfig RUNTIME_CONFIG;
        return RUNTIME_CONFIG;
    }

    RuntimeConfig::RuntimeConfig() {
        threadInitialStackSize = 256;
        threadMaxStackSize = 512;
    }
}
