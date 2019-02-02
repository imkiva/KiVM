//
// Created by kiva on 2018/4/15.
//
#pragma once

#ifdef KIVM_PLATFORM_WINDOWS

#include <shared/os/common/dl.h>

namespace kivm {
    namespace dl {
        class WindowsDLInterface final {
        private:
            DLHandler handler;

        public:
            WindowsDLInterface();

            WindowsDLInterface(const WindowsDLInterface &) = delete;

            explicit WindowsDLInterface(const std::string &file);

            ~WindowsDLInterface();

            bool open(const std::string &file);

            DLSymbol findSymbol(const std::string &symbolName) const;

            void close();
        };
    }
}

#endif
