//
// Created by kiva on 2018/3/29.
//
#pragma once

namespace kivm {
    namespace java {
        namespace lang {
            // JLS 20.20.1-3
            enum ThreadPriority {
                // Initial non-priority value
                    NO_PRIORITY = -1,

                // Minimum priority
                    MIN_PRIORITY = 1,

                // Normal (non-daemon) priority
                    NORMAL_PRIORITY = 5,

                // High priority, used for VMThread
                    NEAR_MAX_PRIORITY = 9,

                // Highest priority, used for WatcherThread, ensures that VMThread doesn't starve profiler
                    MAX_PRIORITY = 10,

                // Critical thread priority
                    CRITICAL_PRIORITY = 11
            };
        }
    }
}
