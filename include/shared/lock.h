//
// Created by kiva on 2018/2/25.
//

#ifndef KIVAVM_LOCK_H
#define KIVAVM_LOCK_H

#include <mutex>

using Lock = std::mutex;
using LockGuard = std::lock_guard<std::mutex>;

#endif //KIVAVM_LOCK_H