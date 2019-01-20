//
// Created by kiva on 2018/4/15.
//
#include <shared/dl.h>

#ifdef KIVM_PLATFORM_WINDOWS

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
            auto handler = DLHandler(::LoadLibrary(file.c_str()));
            this->handler = handler;
            return this->handler != nullptr;
        }

        DLSymbol WindowsDLInterface::findSymbol(const std::string &symbolName) const {
            auto hmod = HMODULE(handler);
            FARPROC sym = ::GetProcAddress(hmod != nullptr ? hmod : GetModuleHandle(nullptr),
                                            symbolName.c_str());
            if (sym != nullptr) {
                return DLSymbol(sym);
            }

            return nullptr;
        }

        void WindowsDLInterface::close() {
            if (handler != nullptr) {
                ::FreeLibrary(HMODULE(handler));
                handler = nullptr;
            }
        }
    }
}

#endif
