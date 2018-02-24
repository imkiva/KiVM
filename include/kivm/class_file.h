//
// Created by kiva on 2018/2/25.
//

#pragma clang diagnostic push
#pragma ide diagnostic ignored "OCUnusedGlobalDeclarationInspection"
#pragma ide diagnostic ignored "OCUnusedStructInspection"
#pragma ide diagnostic ignored "OCUnusedMacroInspection"
#ifndef KIVAVM_CLASS_FILE_H
#define KIVAVM_CLASS_FILE_H

#include "kivm.h"

#define READ_U1(v, p, l)  v = *(p)++
#define READ_U2(v, p, l)  v = ((p)[0]<<8)|(p)[1]; (p)+=2
#define READ_U4(v, p, l)  v = ((p)[0]<<24)|((p)[1]<<16)|((p)[2]<<8)|(p)[3]; (p)+=4
#define READ_U8(v, p, l)  v = ((u8)(p)[0]<<56)|((u8)(p)[1]<<48)|((u8)(p)[2]<<40) \
                            |((u8)(p)[3]<<32)|((u8)(p)[4]<<24)|((u8)(p)[5]<<16) \
                            |((u8)(p)[6]<<8)|(u8)(p)[7]; (p)+=8
#define SKIP_U2(v, p, l)  (p)+=2

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

/**
 * Constant pool info
 */
struct cp_info {
    /**
     * Constant pool tags
     */
    u1 tag;
    u1 *info;
};

struct CONSTANT_Class_info {
    /**
     * Must be {@code CONSTANT_Class}
     */
    u1 tag;

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

struct CONSTANT_Fieldref_info {
    /**
     * Must be {@code CONSTANT_Fieldref}
     */
    u1 tag;

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

struct CONSTANT_Methodref_info {
    /**
     * Must be {@code CONSTANT_Methodref}
     */
    u1 tag;

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

struct CONSTANT_InterfaceMethodref_info {
    /**
     * Must be {@code CONSTANT_InterfaceMethodref}
     */
    u1 tag;

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

struct CONSTANT_String_info {
    /**
     * Must be {@code CONSTANT_String}
     */
    u1 tag;

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

struct CONSTANT_Integer_info {
    /**
     * Must be {@code CONSTANT_Integer}
     */
    u1 tag;

    /**
     * The {@code bytes} represents the value of the int constant.
     * The bytes of the value are stored
     * in big-endian (high byte first) order.
     */
    u4 bytes;
};

struct CONSTANT_Float_info {
    /**
     * Must be {@code CONSTANT_Float}
     */
    u1 tag;

    /**
     * The {@code bytes} represents the value of the float constant
     * in IEEE 754 floating-point single format (§2.3.2).
     * The bytes of the single format representation are stored
     * in big-endian (high byte first) order.
     */
    u4 bytes;
};

struct CONSTANT_Long_info {
    /**
     * Must be {@code CONSTANT_Long}
     */
    u1 tag;

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
};

struct CONSTANT_Double_info {
    /**
     * Must be {@code CONSTANT_Double}
     */
    u1 tag;


    /**
     * The {@code high_bytes} and {@code low_bytes}
     * together represent the double value in
     * IEEE 754 floating-point double format (§2.3.2).
     * The bytes of each item are stored
     * in big-endian (high byte first) order.
     */
    u4 high_bytes;
    u4 low_bytes;
};

struct CONSTANT_NameAndType_info {
    /**
     * Must be {@code CONSTANT_NameAndType}
     */
    u1 tag;

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

struct CONSTANT_Utf8_info {
    /**
     * Must be {@code CONSTANT_Utf8}
     */
    u1 tag;

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
};

struct CONSTANT_MethodHandle_info {
    /**
     * Must be {@code CONSTANT_MethodHandle}
     */
    u1 tag;

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

struct CONSTANT_MethodType_info {
    /**
     * Must be {@code CONSTANT_MethodType}
     */
    u1 tag;

    /**
     * The value of the {@code descriptor_index} item must be
     * a valid index into the {@code constant_pool} table.
     * The {@code constant_pool} entry at that index must be
     * a {@code CONSTANT_Utf8_info} structure (§4.4.7)
     * representing a method descriptor (§4.3.3).
     */
    u2 descriptor_index;
};

struct CONSTANT_InvokeDynamic_info {
    /**
     * Must be {@code CONSTANT_InvokeDynamic}
     */
    u1 tag;

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


/**
 * Attribute info
 */
struct attribute_info {
    /**
     * The value of the {@code attribute_name_index} must be
     * a valid unsigned 16-bit index into the {@code constant_pool} of the class.
     * The {@code constant_pool} entry at attribute_name_index must be
     * a {@code CONSTANT_Utf8_info} structure (§4.4.7)
     * representing the name of the attribute.
     */
    u2 attribute_name_index;

    /**
     * The value of the {@code attribute_length} item indicates
     * the length of the subsequent information in bytes.
     * The length does not include the initial six bytes
     * that contain the {@code attribute_name_index}
     * and {@code attribute_length} items.
     */
    u4 attribute_length;

    /**
     * The attribute info, whose length is {@code attribute_length}
     */
    u1 *info;
};


/**
 * Field info
 */
struct field_info {
    /**
     * The value of the {@code access_flags} item is
     * a mask of flags used to denote access permission to
     * and properties of this field.
     * The interpretation of each flag, when set,
     * is specified in {@file kivm.h}.
     */
    u2 access_flags;

    /**
     * The value of the {@code name_index} item must be
     * a valid index into the {@code constant_pool} table.
     * The {@code constant_pool} entry at that index must be
     * a {@code CONSTANT_Utf8_info} structure (§4.4.7)
     * representing a valid unqualified name denoting a field (§4.2.2).
     */
    u2 name_index;

    /**
     * The value of the {@code name_index} item must be
     * a valid index into the {@code constant_pool} table.
     * The {@code constant_pool} entry at that index must be
     * a {@code CONSTANT_Utf8_info} structure (§4.4.7)
     * representing a valid field descriptor (§4.3.2).
     */
    u2 descriptor_index;

    /**
     * The value of the {@code attributes_count} item indicates
     * the number of additional attributes of this field.
     */
    u2 attributes_count;

    /**
     * The additional attributes of this field (§4.7),
     * whose length is {@code attributes_count}
     */
    attribute_info *attributes;
};


/**
 * Field info
 */
struct method_info {
    /**
     * The value of the {@code access_flags} item is
     * a mask of flags used to denote access permission to
     * and properties of this method.
     * The interpretation of each flag, when set,
     * is specified in {@file kivm.h}.
     */
    u2 access_flags;

    /**
     * The value of the {@code name_index} item must be
     * a valid index into the {@code constant_pool} table.
     * The {@code constant_pool} entry at that index must be
     * a {@code CONSTANT_Utf8_info} structure (§4.4.7)
     * representing either one of the special method names <init> or <clinit> (§2.9),
     * or a valid unqualified name denoting a method (§4.2.2).
     */
    u2 name_index;

    /**
     * The value of the {@code name_index} item must be
     * a valid index into the {@code constant_pool} table.
     * The {@code constant_pool} entry at that index must be
     * a {@code CONSTANT_Utf8_info} structure
     * representing a valid method descriptor (§4.3.3).
     */
    u2 descriptor_index;

    /**
     * The value of the {@code attributes_count} item indicates
     * the number of additional attributes of this field.
     */
    u2 attributes_count;

    /**
     * The additional attributes of this method (§4.7),
     * whose length is {@code attributes_count}
     */
    attribute_info *attributes;
};

struct ClassFileFormat {
    /**
     * The magic item supplies the magic number identifying the class file format.
     * It has the value 0xCAFEBABE.
     */
    u4 magic;
    u2 minor_version;
    u2 major_version;

    u2 constant_pool_count;

    /**
     * The constant_pool table is indexed
     * from {@code 1} to {@code constant_pool_count - 1}.
     */
    cp_info *constant_pool;

    u2 access_flags;

    /**
     * The value of the {@code this_class} item must be
     * a valid index into the {@code constant_pool} table.
     * The {@code constant_pool} entry at that index must be
     * a {@code CONSTANT_Class_info} structure (§4.4.1)
     * representing the class or interface defined by this class file.
     */
    u2 this_class;

    /**
     * For a class, the value of the {@code super_class} item either must
     * be zero or must be a valid index into the {@code constant_pool} table.
     * If it is nonzero, the {@code constant_pool} entry at that index must be
     * a {@code CONSTANT_Class_info} structure representing
     * the direct superclass of the class defined by this class file.
     */
    u2 super_class;

    u2 interfaces_count;
    u2 *interfaces;

    u2 fields_count;
    field_info *fields;

    u2 methods_count;
    method_info *methods;

    u2 attributes_count;
    attribute_info *attributes;
};

#endif //KIVAVM_CLASS_FILE_H

#pragma clang diagnostic pop