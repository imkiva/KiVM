//
// Created by kiva on 2018/2/25.
// Modified by mikecovlee on 2018/2/25.
//

#pragma once

#include <mutex>

namespace kivm {
	using Lock = std::mutex;
	using LockGuard = std::lock_guard<std::mutex>;
}