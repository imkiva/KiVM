//
// Created by kiva on 2018/2/25.
//

#pragma once

#include <kivm/kivm.h>
#include <kivm/classfile/attributeInfo.h>
#include <kivm/classfile/constantPool.h>
#include <iosfwd>

namespace kivm {
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
        attribute_info **attributes;

        field_info();

        ~field_info();

        void init(ClassFileStream &stream, cp_info **constant_pool);
    };


    /**
     * Method info
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
        attribute_info **attributes;

        method_info();

        ~method_info();

        void init(ClassFileStream &stream, cp_info **constant_pool);
    };

    struct ClassFile final {
        /**
         * The magic item supplies the magic number identifying the class file format.
         * It has the value 0xCAFEBABE.
         */
        u4 magic{};
        u2 minor_version{};
        u2 major_version{};

        u2 constant_pool_count{};

        /**
         * The constant_pool table is indexed
         * from {@code 1} to {@code constant_pool_count - 1}.
         */
        cp_info **constant_pool = nullptr;

        u2 access_flags{};

        /**
         * The value of the {@code this_class} item must be
         * a valid index into the {@code constant_pool} table.
         * The {@code constant_pool} entry at that index must be
         * a {@code CONSTANT_Class_info} structure (§4.4.1)
         * representing the class or interface defined by this class file.
         */
        u2 this_class{};

        /**
         * For a class, the value of the {@code super_class} item either must
         * be zero or must be a valid index into the {@code constant_pool} table.
         * If it is nonzero, the {@code constant_pool} entry at that index must be
         * a {@code CONSTANT_Class_info} structure representing
         * the direct superclass of the class defined by this class file.
         */
        u2 super_class{};

        u2 interfaces_count{};
        u2 *interfaces = nullptr;

        u2 fields_count{};
        field_info *fields = nullptr;

        u2 methods_count{};
        method_info *methods = nullptr;

        u2 attributes_count{};
        attribute_info **attributes = nullptr;
    };
}
