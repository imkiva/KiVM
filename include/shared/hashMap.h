//
// Created by kiva on 2018/6/12.
//

#pragma once

#if defined(KIVM_PLATFORM_APPLE)

#include <unordered_map>

namespace kivm {
    template<typename K, typename V>
    using HashMap = std::unordered_map<K, V>;
}

#else

#include <sparsepp/spp.h>

namespace kivm {
    template <typename K, typename V>
    using HashMap = spp::sparse_hash_map<K, V>;
}

#endif
