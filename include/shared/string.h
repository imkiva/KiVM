//
// Created by kiva on 2018/2/25.
//
#pragma once

#include <shared/types.h>
#include <string>

namespace kivm {

    namespace strings {
        using String  = std::wstring;

        String from_bytes(u1 *bytes, size_t length);

        String from_std_string(const std::string &str);

        std::string to_std_string(const String &str);
    }

    using String = strings::String;
}
