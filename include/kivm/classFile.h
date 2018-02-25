//
// Created by kiva on 2018/2/25.
//

#pragma clang diagnostic push
#pragma ide diagnostic ignored "OCUnusedGlobalDeclarationInspection"
#pragma ide diagnostic ignored "OCUnusedStructInspection"
#pragma ide diagnostic ignored "OCUnusedMacroInspection"
#pragma once

#include <kivm/kivm.h>
#include <kivm/constantPool.h>
#include <iosfwd>

namespace kivm {
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


/**
 * Ugly but useful
 */
#define READ_U2(v, p)  v = ((p)[0]<<8)|(p)[1];
#define READ_U4(v, p)  v = ((p)[0]<<24)|((p)[1]<<16)|((p)[2]<<8)|(p)[3];
#define READ_U8(v, p)  v = ((u8)(p)[0]<<56)|((u8)(p)[1]<<48)|((u8)(p)[2]<<40) \
                            |((u8)(p)[3]<<32)|((u8)(p)[4]<<24)|((u8)(p)[5]<<16) \
                            |((u8)(p)[6]<<8)|(u8)(p)[7];


    /**
     *  Input stream for reading .class file
     *  The entire input stream is present in a buffer allocated by the caller.
     *  The caller is responsible for deallocating the buffer.
     */
    class ClassFileStream {
    private:
        u1 *_buffer_start; // Buffer bottom
        u1 *_buffer_end;   // Buffer top (one past last element)
        u1 *_current;      // Current buffer position
        char *_source;       // Source of stream (directory name, ZIP/JAR archive name)
        bool _need_verify;  // True if verification is on for the class file

        void guarantee_more(int size) {
            auto remaining = (size_t) (_buffer_end - _current);
            auto usize = (unsigned int) size;
            if (usize > remaining) {
                // TODO: Unexpected EOF
            }
        }

    public:
        ClassFileStream(u1 *buffer, int length, char *source);

        // Buffer access
        u1 *buffer() const { return _buffer_start; }

        int length() const { return static_cast<int>(_buffer_end - _buffer_start); }

        u1 *current() const { return _current; }

        void set_current(u1 *pos) { _current = pos; }

        char *source() const { return _source; }

        void set_verify(bool flag) { _need_verify = flag; }

        // Read u1 from stream
        u1 get_u1();

        u1 get_u1_fast() {
            return *_current++;
        }

        // Read u2 from stream
        u2 get_u2();

        u2 get_u2_fast() {
            u2 res;
            READ_U2(res, _current);
            _current += 2;
            return res;
        }

        // Read u4 from stream
        u4 get_u4();

        u4 get_u4_fast() {
            u4 res;
            READ_U4(res, _current);
            _current += 4;
            return res;
        }

        // Read u8 from stream
        u8 get_u8();

        u8 get_u8_fast() {
            u8 res;
            READ_U8(res, _current);
            _current += 8;
            return res;
        }

        // Copy `count` u1 bytes from current position to `to`
        void get_u1_bytes(u1 *to, int count);

        // Get direct pointer into stream at current position.
        // Returns NULL if length elements are not remaining. The caller is
        // responsible for calling skip below if buffer contents is used.
        u1 *get_u1_buffer() {
            return _current;
        }

        u2 *get_u2_buffer() {
            return (u2 *) _current;
        }

        // Skip length u1 or u2 elements from stream
        void skip_u1(int length);

        void skip_u1_fast(int length) {
            _current += length;
        }

        void skip_u2(int length);

        void skip_u2_fast(int length) {
            _current += 2 * length;
        }

        void skip_u4(int length);

        void skip_u4_fast(int length) {
            _current += 4 * length;
        }

        // Tells whether eos is reached
        bool at_eos() const { return _current == _buffer_end; }

        // Yeah...
        ClassFileStream &operator>>(CONSTANT_Utf8_info &info);

        ClassFileStream &operator>>(CONSTANT_Class_info &info);

        ClassFileStream &operator>>(CONSTANT_Double_info &info);

        ClassFileStream &operator>>(CONSTANT_Float_info &info);

        ClassFileStream &operator>>(CONSTANT_Long_info &info);

        ClassFileStream &operator>>(CONSTANT_Integer_info &info);

        ClassFileStream &operator>>(CONSTANT_String_info &info);

        ClassFileStream &operator>>(CONSTANT_Fieldref_info &info);

        ClassFileStream &operator>>(CONSTANT_Methodref_info &info);

        ClassFileStream &operator>>(CONSTANT_InterfaceMethodref_info &info);

        ClassFileStream &operator>>(CONSTANT_MethodHandle_info &info);

        ClassFileStream &operator>>(CONSTANT_MethodType_info &info);

        ClassFileStream &operator>>(CONSTANT_NameAndType_info &info);

        ClassFileStream &operator>>(CONSTANT_InvokeDynamic_info &info);
    };

    class ClassFileParser {
    public:
    };

}

#pragma clang diagnostic pop