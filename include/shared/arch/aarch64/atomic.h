//
// Created by kiva on 2018/5/13.
//
#pragma once

#include <cstdint>
#include <compileTimeConfig.h>
#include <kivm/kivm.h>

#if KIVM_ARCH_aarch64

#define mbarrier()  asm volatile("dmb ish" ::: "memory")

static inline unsigned long __cmpxchg(volatile void *ptr, unsigned long old,
                                      unsigned long newValue, int size) {
    unsigned long oldval = 0, res;
    switch (size) {
        case 1:
            do {
                asm volatile("// __cmpxchg1\n"
                             "	ldxrb	%w1, %2\n"
                             "	mov	%w0, #0\n"
                             "	cmp	%w1, %w3\n"
                             "	b.ne	1f\n"
                             "	stxrb	%w0, %w4, %2\n"
                             "1:\n"
                : "=&r" (res), "=&r" (oldval), "+Q" (*(uint8_t *) ptr)
                : "Ir" (old), "r" (newValue)
                : "cc");
            } while (res);
            break;
        case 2:
            do {
                asm volatile("// __cmpxchg2\n"
                             "	ldxrh	%w1, %2\n"
                             "	mov	%w0, #0\n"
                             "	cmp	%w1, %w3\n"
                             "	b.ne	1f\n"
                             "	stxrh	%w0, %w4, %2\n"
                             "1:\n"
                : "=&r" (res), "=&r" (oldval), "+Q" (*(uint16_t *) ptr)
                : "Ir" (old), "r" (newValue)
                : "cc");
            } while (res);
            break;
        case 4:
            do {
                asm volatile("// __cmpxchg4\n"
                             "	ldxr	%w1, %2\n"
                             "	mov	%w0, #0\n"
                             "	cmp	%w1, %w3\n"
                             "	b.ne	1f\n"
                             "	stxr	%w0, %w4, %2\n"
                             "1:\n"
                : "=&r" (res), "=&r" (oldval), "+Q" (*(uint32_t *) ptr)
                : "Ir" (old), "r" (newValue)
                : "cc");
            } while (res);
            break;
        case 8:
            do {
                asm volatile("// __cmpxchg8\n"
                             "	ldxr	%1, %2\n"
                             "	mov	%w0, #0\n"
                             "	cmp	%1, %3\n"
                             "	b.ne	1f\n"
                             "	stxr	%w0, %4, %2\n"
                             "1:\n"
                : "=&r" (res), "=&r" (oldval), "+Q" (*(uint64_t *) ptr)
                : "Ir" (old), "r" (newValue)
                : "cc");
            } while (res);
            break;
        default:
            SHOULD_NOT_REACH_HERE();
    }
    return oldval;
}

static inline unsigned long __cmpxchg_mb(volatile void *ptr, unsigned long old,
                                         unsigned long newValue, int size) {
    unsigned long ret;
    mbarrier();
    ret = __cmpxchg(ptr, old, newValue, size);
    mbarrier();
    return ret;
}

#define cmpxchg(ptr, o, n)                        \
    ((__typeof__(*(ptr)))__cmpxchg_mb((ptr),            \
                      (unsigned long)(o),        \
                      (unsigned long)(n),        \
                      sizeof(*(ptr))))

#endif
