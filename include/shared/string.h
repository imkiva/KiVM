//
// Created by kiva on 2018/2/25.
//
#pragma once

#include <shared/types.h>
#include <string>

namespace kivm {

    namespace strings {
        using String  = std::wstring;

        String fromBytes(u1 *bytes, size_t length);

        String fromStdString(const std::string &str);

        String replaceAll(const String &string, const String &oldValue, const String &newValue);

        std::string toStdString(const String &str);
    }

    using String = strings::String;
}
