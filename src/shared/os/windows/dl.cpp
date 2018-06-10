//
// Created by kiva on 2018/4/15.
//

#ifdef KIVM_PLATFORM_WINDOWS

#include <shared/os/windows/dl.h>
#include <windows.h>

namespace kivm {
    namespace dl {

        WindowsDLInterface::WindowsDLInterface() : handler(nullptr) {

        }

        WindowsDLInterface::WindowsDLInterface(const std::string &file) : handler(nullptr) {
            open(file);
        }

        WindowsDLInterface::~WindowsDLInterface() {
            close();
        }

        bool WindowsDLInterface::open(const std::string &file) {
            DLHandler handler = (DLHandler) ::LoadLibrary(file.c_str());
            this->handler = handler;
            return this->handler != nullptr;
        }

        DLSymbol WindowsDLInterface::findSymbol(const std::string &symbolName) const {
            DLSymbol sym = ::GetProcAddress(handler != nullptr ? handler : GetModuleHandle(nullptr),
                                            symbolName.c_str());
            if (sym != nullptr) {
                return sym;
            }

            return nullptr;
        }

        void WindowsDLInterface::close() {
            if (handler != nullptr) {
                ::FreeLibrary(handler);
                handler = nullptr;
            }
        }
    }
}

#endif
