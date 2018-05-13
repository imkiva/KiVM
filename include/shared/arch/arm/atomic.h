//
// Created by kiva on 2018/5/13.
//
#pragma once

#include <compileTimeConfig.h>

#if KIVM_ARCH_arm

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

#define COMPARE_AND_SWAP(addr, old_val, new_val)          \
        COMPARE_AND_SWAP_32(addr, old_val, new_val)

#define LOCKWORD_READ(addr) *addr
#define LOCKWORD_WRITE(addr, value) *(addr) = value
#define LOCKWORD_COMPARE_AND_SWAP(addr, old_val, new_val) \
        COMPARE_AND_SWAP(addr, old_val, new_val)

#endif
