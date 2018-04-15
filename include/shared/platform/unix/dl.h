//
// Created by kiva on 2018/4/15.
//
#pragma once

#ifdef KIVM_PLATFORM_UNIX

#include <shared/platform/common/dl.h>

namespace kivm {
    namespace dl {
        class UnixDLInterface {
        private:
            DLHandler handler;

        public:
            UnixDLInterface();

            UnixDLInterface(const UnixDLInterface &) = delete;

            explicit UnixDLInterface(const std::string &file);

            ~UnixDLInterface();

            bool open(const std::string &file);

            DLSymbol findSymbol(const std::string &symbolName) const;

            void close();
        };
    }
}

#endif
