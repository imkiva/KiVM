//
// Created by kiva on 2018/3/25.
//
#pragma once

namespace kivm {
    struct RuntimeConfig {
        int threadInitialStackSize;
        int threadMaxStackSize;

        static void init();
        static RuntimeConfig& get();
    };
}

