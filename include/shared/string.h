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
    }


    using String = strings::String;
}
