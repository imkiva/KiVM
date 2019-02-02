//
// Created by kiva on 2018/11/11.
//
#pragma once

#include <shared/string.h>
#include <shared/dl.h>

namespace kivm {
    class JavaNativeLibrary final {
    private:
        dl::DLInterface _sharedLibrary;
        String _libraryName;
        bool _linked;

        String findLibrary(const String &libraryName);

    public:
        explicit JavaNativeLibrary(const String &libraryName);

        ~JavaNativeLibrary();

        bool prepare();

        void dispose();
    };
}
