//
// Created by kiva on 2018/2/25.
//

#ifndef KIVAVM_H
#define KIVAVM_H

#include <cstdint>
#include <cstddef>

typedef unsigned char u1;
typedef unsigned short u2;
typedef unsigned int u4;
typedef unsigned long long u8;

/**
 * Declared public; may be accessed from outside its package.
 */
#define ACC_PUBLIC              0x0001

/**
 * Declared private; usable only within the defining class.
 */
#define ACC_PRIVATE             0x0002

/**
 * Declared protected; may be accessed within subclasses.
 */
#define ACC_PROTECTED           0x0004

/**
 * Declared static.
 */
#define ACC_STATIC              0x0008

/**
 * Declared final; never directly assigned to after object construction (JLS §17.5).
 */
#define ACC_FINAL               0x0010

/**
 * Declared synchronized; invocation is wrapped by a monitor use.
 */
#define ACC_SYNCHRONIZED        0x0020
#define ACC_SUPER               0x0020

/**
 * Declared volatile; cannot be cached.
 */
#define ACC_VOLATILE            0x0040

/**
 * A bridge method, generated by the compiler.
 */
#define ACC_BRIDGE              0x0040

/**
 * Declared with variable number of arguments.
 */
#define ACC_VARARGS             0x0080

/**
 * Declared transient; not written or read by a persistent object manager.
 */
#define ACC_TRANSIENT           0x0080

/**
 * Declared native; implemented in a language other than Java.
 */
#define ACC_NATIVE              0x0100

/**
 * Was an interface in source.
 */
#define ACC_INTERFACE           0x0200

/**
 * Declared abstract; no implementation is provided.
 */
#define ACC_ABSTRACT            0x0400

/**
 * Declared strictfp; floating-point mode is FP- strict.
 */
#define ACC_STRICT              0x0800

/**
 * Declared synthetic; not present in the source code.
 */
#define ACC_SYNTHETIC           0x1000

/**
 * Declared as an annotation type.
 */
#define ACC_ANNOTATION          0x2000

/**
 * Declared as an element of an enum.
 */
#define ACC_ENUM                0x4000

#define ACC_MIRANDA             0x8000
#define ACC_REFLECT_MASK        0xffff

#endif //KIVAVM_H
