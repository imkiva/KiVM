//
// Created by kiva on 2018/5/13.
//
#pragma once

#include <compileTimeConfig.h>

#if KIVM_ARCH_arm

/*
 * Use the __kuser_memory_barrier helper in the CPU helper page. See
 * arch/arm/kernel/entry-armv.S in the kernel source for details.
 */
#define mbarrier() ((void(*)(void))0xffff0fa0)()

#define COMPARE_AND_SWAP_32(addr, old_val, new_val)       \
({                                                        \
    int result, read_val;                                 \
    __asm__ __volatile__ ("                               \
        1:      mov %0, #0;                               \
                ldrex %1, [%2];                           \
                cmp %3, %1;                               \
                bne 2f;                                   \
                strex %0, %4, [%2];                       \
                cmp %0, #1;                               \
                beq 1b;                                   \
                mov %0, #1;                               \
        2:"                                               \
    : "=&r" (result), "=&r" (read_val)                    \
    : "r" (addr), "r" (old_val), "r" (new_val)            \
    : "cc", "memory");                                    \
    result;                                               \
})

#define cmpxchg(addr, old_val, new_val)          \
        COMPARE_AND_SWAP_32(addr, old_val, new_val)

#endif
