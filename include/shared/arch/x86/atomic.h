//
// Created by kiva on 2018/5/13.
//
#pragma once

#include <compileTimeConfig.h>

#if KIVM_ARCH_x86 || KIVM_ARCH_x86_64

#if KIVM_ARCH_x86_64
#define mbarrier() asm volatile("mfence":::"memory")
#define LOCK_PREFIX "\\n\\tlock; "
#else
#define mbarrier() asm volatile("lock; addl $0,0(%%esp)" ::: "memory")
#endif

#define __X86_CASE_B 1
#define __X86_CASE_W 2
#define __X86_CASE_L 4
#ifdef KIVM_ARCH_x86_64
#define __X86_CASE_Q 8
#else
#define __X86_CASE_Q (-1)
#endif

#define __raw_cmpxchg(ptr, old, new_, size, lock) \
({   \
    __typeof__(*(ptr)) __ret;         \
    __typeof__(*(ptr)) __old = (old);     \
    __typeof__(*(ptr)) __new = (new_);     \
    switch (size) {      \
    case __X86_CASE_B:  \
    {          \
        volatile u8 *__ptr = (volatile uint8_t *)(ptr);        \
        asm volatile(lock "cmpxchgb %2,%1" \
      : "=a" (__ret), "+m" (*__ptr)        \
      : "q" (__new), "0" (__old) \
      : "memory");     \
        break;      \
    }          \
    case __X86_CASE_W:  \
    {          \
        volatile u16 *__ptr = (volatile uint16_t *)(ptr);        \
        asm volatile(lock "cmpxchgw %2,%1" \
      : "=a" (__ret), "+m" (*__ptr)        \
      : "r" (__new), "0" (__old) \
      : "memory");     \
        break;      \
    }          \
    case __X86_CASE_L:  \
    {          \
        volatile u32 *__ptr = (volatile uint32_t *)(ptr);        \
        asm volatile(lock "cmpxchgl %2,%1" \
      : "=a" (__ret), "+m" (*__ptr)        \
      : "r" (__new), "0" (__old) \
      : "memory");     \
        break;      \
    }          \
    case __X86_CASE_Q:  \
    {          \
        volatile u64 *__ptr = (volatile uint64_t *)(ptr);        \
        asm volatile(lock "cmpxchgq %2,%1" \
      : "=a" (__ret), "+m" (*__ptr)        \
      : "r" (__new), "0" (__old) \
      : "memory");     \
        break;      \
    }          \
    default:      \
        SHOULD_NOT_REACH_HERE();         \
    }          \
    __ret;          \
})

#define __cmpxchg(ptr, old, new_, size)         \
    __raw_cmpxchg((ptr), (old), (new_), (size), LOCK_PREFIX)

#define cmpxchg(ptr, old, new_)  \
    __cmpxchg(ptr, old, new_, sizeof(*(ptr)))

#endif
