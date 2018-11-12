//
// Created by kiva on 2018/11/11.
//
#pragma once

#include <shared/string.h>
#include <shared/dl.h>

namespace kivm {
    class JavaNativeLibrary {
    private:
        dl::DLInterface sharedLibrary;
        String libraryName;
        bool linked;

        String findLibrary(const String &libraryName);

    public:
        explicit JavaNativeLibrary(const String &libraryName);

        ~JavaNativeLibrary();

        bool openAndLink();

        void unlinkAndClose();
    };
}
