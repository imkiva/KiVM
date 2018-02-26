//
// Created by kiva on 2018/2/25.
//

#pragma once

#include <kivm/kivm.h>

/* Constant pool tags */
#define CONSTANT_Utf8                    1
#define CONSTANT_Integer                 3
#define CONSTANT_Float                   4
#define CONSTANT_Long                    5
#define CONSTANT_Double                  6
#define CONSTANT_Class                   7
#define CONSTANT_String                  8
#define CONSTANT_Fieldref                9
#define CONSTANT_Methodref              10
#define CONSTANT_InterfaceMethodref     11
#define CONSTANT_NameAndType            12
#define CONSTANT_MethodHandle           15
#define CONSTANT_MethodType             16
#define CONSTANT_InvokeDynamic          18

#define REF_getField                 1
#define REF_getStatic                2
#define REF_putField                 3
#define REF_putStatic                4
#define REF_invokeVirtual            5
#define REF_invokeStatic             6
#define REF_invokeSpecial            7
#define REF_newInvokeSpecial         8
#define REF_invokeInterface          9

/* JVM Specification §4.4.4 */
#define _FLOAT_POSITIVE_INFINITY    0x7f800000
#define _FLOAT_NEGATIVE_INFINITY    0xff800000
#define FLOAT_IS_POSITIVE_INFINITY(x)    ((x) == _FLOAT_POSITIVE_INFINITY)
#define FLOAT_IS_NEGATIVE_INFINITY(x)    ((x) == _FLOAT_NEGATIVE_INFINITY)
#define FLOAT_NAN                   0x7f800001
#define FLOAT_IS_NAN(x) (((x) >= 0x7f800001 && (x) <= 0x7fffffff) \
                            || ((x) >= 0xff800001 && (x) <= 0xffffffff))

/* JVM Specification §4.4.5 */
#define _DOUBLE_POSITIVE_INFINITY   0x7ff0000000000000L
#define _DOUBLE_NEGATIVE_INFINITY   0xfff0000000000000L
#define DOUBLE_IS_POSITIVE_INFINITY(x)    ((x) == _DOUBLE_POSITIVE_INFINITY)
#define DOUBLE_IS_NEGATIVE_INFINITY(x)    ((x) == _DOUBLE_NEGATIVE_INFINITY)
#define DOUBLE_NAN                  0x7ff0000000000001L
#define DOUBLE_IS_NAN(x) (((x) >= 0x7ff0000000000001L && (x) <= 0x7fffffffffffffffL) \
                            || ((x) >= 0xfff0000000000001L && (x) <= 0xffffffffffffffffL))

namespace kivm {
    /**
     * Constant pool info
     */
    struct cp_info {
        /**
         * Constant pool tags
         */
        u1 tag;

        cp_info() = default;

        virtual ~cp_info() = default;
    };

    struct CONSTANT_Class_info : public cp_info {
        /**
         * The value of the {@code name_index} item must be
         * a valid index into the {@code constant_pool} table.
         * The {@code constant_pool} entry at that index must be
         * a {@code CONSTANT_Utf8_info} structure (§4.4.7)
         * representing a valid binary class or interface name
         * encoded in internal form (§4.2.1).
         *
         * But for array classes, the name of the class
         * is the descriptor of the array type (§4.3.2).
         */
        u2 name_index;
    };

    struct CONSTANT_Fieldref_info : public cp_info {
        /**
         * The value of the {@code class_index} item must be
         * a valid index into the {@code constant_pool} table.
         * The {@code constant_pool} entry at that index must be
         * a {@code CONSTANT_Class_info} structure (§4.4.1)
         * representing a class or interface type
         * that has the field or method as a member.
         */
        u2 class_index;

        /**
         * The value of the {@code class_index} item must be
         * a valid index into the {@code constant_pool} table.
         * The {@code constant_pool} entry at that index must be
         * a {@code CONSTANT_NameAndType_info} structure (§4.4.6).
         * This {@code constant_pool} entry indicates
         * the name and descriptor of the field.
         *
         * The indicated descriptor must be a field descriptor (§4.3.2).
         */
        u2 name_and_type_index;
    };

    struct CONSTANT_Methodref_info : public cp_info {
        /**
         * The value of the {@code class_index} item must be
         * a valid index into the {@code constant_pool} table.
         * The {@code constant_pool} entry at that index must be
         * a {@code CONSTANT_Class_info} structure (§4.4.1)
         * representing a class type (not an interface type)
         * that has the field or method as a member.
         */
        u2 class_index;

        /**
         * The value of the {@code class_index} item must be
         * a valid index into the {@code constant_pool} table.
         * The {@code constant_pool} entry at that index must be
         * a {@code CONSTANT_NameAndType_info} structure (§4.4.6).
         * This {@code constant_pool} entry indicates
         * the name and descriptor of the method.
         *
         * The indicated descriptor must be a method descriptor (§4.3.3).
         */
        u2 name_and_type_index;
    };

    struct CONSTANT_InterfaceMethodref_info : public cp_info {
        /**
         * The value of the {@code class_index} item must be
         * a valid index into the {@code constant_pool} table.
         * The {@code constant_pool} entry at that index must be
         * a {@code CONSTANT_Class_info} structure (§4.4.1)
         * representing an interface type (not a class type)
         * that has the field or method as a member.
         */
        u2 class_index;

        /**
         * The value of the {@code class_index} item must be
         * a valid index into the {@code constant_pool} table.
         * The {@code constant_pool} entry at that index must be
         * a {@code CONSTANT_NameAndType_info} structure (§4.4.6).
         * This {@code constant_pool} entry indicates
         * the name and descriptor of the method.
         *
         * The indicated descriptor must be a method descriptor (§4.3.3).
         */
        u2 name_and_type_index;
    };

    struct CONSTANT_String_info : public cp_info {
        /**
         * The value of the {@code string_index} item must be
         * a valid index into the {@code constant_pool} table.
         * The {@code constant_pool} entry at that index must be
         * a {@code CONSTANT_Utf8_info} structure (§4.4.7)
         * representing the sequence of Unicode code points
         * to which the String object is to be initialized.
         */
        u2 string_index;
    };

    struct CONSTANT_Integer_info : public cp_info {
        /**
         * The {@code bytes} represents the value of the int constant.
         * The bytes of the value are stored
         * in big-endian (high byte first) order.
         */
        u4 bytes;

        int get_constant() const;
    };

    struct CONSTANT_Float_info : public cp_info {
        /**
         * The {@code bytes} represents the value of the float constant
         * in IEEE 754 floating-point single format (§2.3.2).
         * The bytes of the single format representation are stored
         * in big-endian (high byte first) order.
         */
        u4 bytes;

        float get_constant() const;
    };

    struct CONSTANT_Long_info : public cp_info {
        /**
         * The unsigned {@code high_bytes} and {@code low_bytes}
         * together represent the value of the long constant:
         * {@code ((long) high_bytes << 32) + low_bytes}
         *
         * The bytes of each item are stored
         * in big-endian (high byte first) order.
         */
        u4 high_bytes;
        u4 low_bytes;

        long get_constant() const;
    };

    struct CONSTANT_Double_info : public cp_info {
        /**
         * The {@code high_bytes} and {@code low_bytes}
         * together represent the double value in
         * IEEE 754 floating-point double format (§2.3.2).
         * The bytes of each item are stored
         * in big-endian (high byte first) order.
         */
        u4 high_bytes;
        u4 low_bytes;

        double get_constant() const;
    };

    struct CONSTANT_NameAndType_info : public cp_info {
        /**
         * The value of the {@code name_index} item must be
         * a valid index into the {@code constant_pool} table.
         * The {@code constant_pool} entry at that index must be
         * a {@code CONSTANT_Utf8_info} structure (§4.4.7)
         * representing either the special method name <init> (§2.9)
         * or a valid unqualified name denoting a field or method (§4.2.2).
         */
        u2 name_index;

        /**
         * The value of the {@code descriptor_index} item must be
         * a valid index into the {@code constant_pool} table.
         * The {@code constant_pool} entry at that index must be
         * a {@code CONSTANT_Utf8_info} structure (§4.4.7)
         * representing a valid field descriptor or method descriptor (§4.3.2, §4.3.3).
         */
        u2 descriptor_index;
    };

    struct CONSTANT_Utf8_info : public cp_info {
        /**
         * The value of the {@code length} item gives
         * the number of bytes in the bytes array
         * (not the length of the resulting string).
         * The strings in the {@code CONSTANT_Utf8_info} structure
         * are not null-terminated.
         */
        u2 length;

        /**
         * The bytes array (whose length is {@code length})
         * contains the bytes of the string. No byte may have the value
         * {@code (byte)0} or lie in the range {@code (byte)0xf0} - {@code (byte)0xff}.
         */
        u1 *bytes;

    private:
        /**
         * Make a cache because we may call get_constant() many times.
         */
        String _cached_string;
        bool _cached;

    public:

        CONSTANT_Utf8_info();

        ~CONSTANT_Utf8_info() override;

        String get_constant();
    };

    struct CONSTANT_MethodHandle_info : public cp_info {
        /**
         * The value of the {@code reference_kind} item must be in the range 1 to 9.
         * The value denotes the kind of this method handle,
         * which characterizes its bytecode behavior (§5.4.3.5).
         */
        u1 reference_kind;

        /**
         * The value of the {@code reference_index} item must be
         * a valid index into the {@code constant_pool} table.
         *
         * The constant_pool entry at that index must be as follows:
         *
         *      1)  If the {@code reference_kind} is 1 (REF_getField), 2 (REF_getStatic),
         *          3 (REF_putField), or 4 (REF_putStatic), then the {@code constant_pool}
         *          entry at that index must be a {@code CONSTANT_Fieldref_info} (§4.4.2) structure
         *          representing a field for which a method handle is to be created.
         *
         *      2)  If the {@code reference_kind} is 5 (REF_invokeVirtual) or 8 (REF_newInvokeSpecial),
         *          then the {@code constant_pool} entry at that index must be
         *          a {@code CONSTANT_Methodref_info} structure (§4.4.2)
         *          representing a class's method or constructor (§2.9)
         *          for which a method handle is to be created.
         *
         *      3)  If the {@code reference_kind} is 6 (REF_invokeStatic) or 7 (REF_invokeSpecial),
         *          then if the class file version number is less than 52.0,
         *          the {@code constant_pool} entry at that index must be
         *          a {@code CONSTANT_Methodref_info} structure representing a class's method
         *          for which a method handle is to be created;
         *
         *          if the class file version number is 52.0 or above,
         *          the {@code constant_pool} entry at that index must be
         *          either a {@code CONSTANT_Methodref_info} structure
         *          or a {@code CONSTANT_InterfaceMethodref_info} structure (§4.4.2)
         *          representing a class's or interface's method
         *          for which a method handle is to be created.
         *
         *      4)  If the {@code reference_kind} is 9 (REF_invokeInterface),
         *          then the {@code constant_pool} entry at that index must be
         *          a {@code CONSTANT_InterfaceMethodref_info} structure
         *          representing an interface's method
         *          for which a method handle is to be created.
         *
         *      5)  If the {@code reference_kind} is 5 (REF_invokeVirtual), 6 (REF_invokeStatic),
         *          7 (REF_invokeSpecial), or 9 (REF_invokeInterface), the name of the method
         *          represented by a {@code CONSTANT_Methodref_info} structure
         *          or a {@code CONSTANT_InterfaceMethodref_info} structure must
         *          not be <init> or <clinit>.
         *
         *      6)  If the {@code reference_kind} is 8 (REF_newInvokeSpecial),
         *          the name of the method represented by a {@code CONSTANT_Methodref_info} structure
         *          must be <init>.
         *
         */
        u2 reference_index;
    };

    struct CONSTANT_MethodType_info : public cp_info {
        /**
         * The value of the {@code descriptor_index} item must be
         * a valid index into the {@code constant_pool} table.
         * The {@code constant_pool} entry at that index must be
         * a {@code CONSTANT_Utf8_info} structure (§4.4.7)
         * representing a method descriptor (§4.3.3).
         */
        u2 descriptor_index;
    };

    struct CONSTANT_InvokeDynamic_info : public cp_info {
        /**
         * The value of the {@code bootstrap_method_attr_index} item must be
         * a valid index into the {@code bootstrap_methods} array of
         * the bootstrap method table (§4.7.23) of this class file.
         */
        u2 bootstrap_method_attr_index;

        /**
         * The value of the {@code name_and_type_index} item must be
         * a valid index into the {@code constant_pool} table.
         * The {@code constant_pool} entry at that index must be
         * a {@code CONSTANT_NameAndType_info} structure (§4.4.6)
         * representing a method name and method descriptor (§4.3.3).
         */
        u2 name_and_type_index;
    };
}
