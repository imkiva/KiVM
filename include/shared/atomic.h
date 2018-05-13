//
// Created by kiva on 2018/5/13.
//
#pragma once

#include <compileTimeConfig.h>

#if KIVM_ARCH_arm
#include <shared/arch/arm/atomic.h>
#elif KIVM_ARCH_aarch64
#include <shared/arch/aarch64/atomic.h>
#elif KIVM_ARCH_x86
#include <shared/arch/x86/atomic.h>
#elif KIVM_ARCH_x86_64
#include <shared/arch/x86_64/atomic.h>
#endif
