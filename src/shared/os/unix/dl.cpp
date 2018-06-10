//
// Created by kiva on 2018/4/15.
//

#if defined(KIVM_PLATFORM_UNIX) || defined(KIVM_PLATFORM_APPLE)

// for RTLD_DEFAULT
#ifndef _GNU_SOURCE
#define _GNU_SOURCE
#endif

#include <shared/os/unix/dl.h>
#include <dlfcn.h>

namespace kivm {
    namespace dl {
        UnixDLInterface::UnixDLInterface() : handler(nullptr) {
        }

        UnixDLInterface::UnixDLInterface(const std::string &file) : handler(nullptr) {
            open(file);
        }

        UnixDLInterface::~UnixDLInterface() {
            close();
        }

        bool UnixDLInterface::open(const std::string &file) {
            DLHandler handler = ::dlopen(file.c_str(), RTLD_LAZY);
            this->handler = handler;
            return this->handler != nullptr;
        }

        DLSymbol UnixDLInterface::findSymbol(const std::string &symbolName) const {
            // Clear errors
            (void) ::dlerror();

            DLSymbol sym = dlsym(handler != nullptr ? handler : RTLD_DEFAULT, symbolName.c_str());
            if (::dlerror() == nullptr) {
                return sym;
            }
            return nullptr;
        }

        void UnixDLInterface::close() {
            if (handler != nullptr) {
                ::dlclose(handler);
                handler = nullptr;
            }
        }
    }
}

#endif