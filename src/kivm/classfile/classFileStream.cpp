//
// Created by kiva on 2018/2/25.
//

#include <kivm/classfile/classFileStream.h>
#include <cassert>

#pragma clang diagnostic push
#pragma ide diagnostic ignored "missing_default_case"
namespace kivm {

    ClassFileStream::ClassFileStream() {
        this->_current = nullptr;
        this->_buffer_start = nullptr;
        this->_buffer_end = nullptr;
        this->_source = nullptr;
    }

    void ClassFileStream::init(u1 *buffer, size_t length) {
        this->_buffer_start = buffer;
        this->_buffer_end = buffer + length;
        set_verify(false);
        set_current(buffer);
    }

    u1 ClassFileStream::get_u1() {
        if (_need_verify) {
            guarantee_more(1);
        } else {
            assert(1 <= _buffer_end - _current);
        }
        return get_u1_fast();
    }

    u2 ClassFileStream::get_u2() {
        if (_need_verify) {
            guarantee_more(2);
        } else {
            assert(2 <= _buffer_end - _current);
        }
        return get_u2_fast();
    }

    u4 ClassFileStream::get_u4() {
        if (_need_verify) {
            guarantee_more(4);
        } else {
            assert(4 <= _buffer_end - _current);
        }
        return get_u4_fast();
    }

    u8 ClassFileStream::get_u8() {
        if (_need_verify) {
            guarantee_more(8);
        } else {
            assert(8 <= _buffer_end - _current);
        }
        return get_u8_fast();
    }

    void ClassFileStream::skip_u1(int length) {
        if (_need_verify) {
            guarantee_more(length);
        }
        _current += length;
    }

    void ClassFileStream::skip_u2(int length) {
        if (_need_verify) {
            guarantee_more(length * 2);
        }
        _current += length * 2;
    }

    void ClassFileStream::skip_u4(int length) {
        if (_need_verify) {
            guarantee_more(length * 4);
        }
        _current += length * 4;
    }

    void ClassFileStream::get_u1_bytes(u1 *to, int count) {
        u1 *from = get_u1_buffer();
        skip_u1(count);

        int n = (count + 7) / 8;
        switch (count % 8) {
            case 0:
                do {
                    *to++ = *from++;
                    case 7:
                        *to++ = *from++;
                    case 6:
                        *to++ = *from++;
                    case 5:
                        *to++ = *from++;
                    case 4:
                        *to++ = *from++;
                    case 3:
                        *to++ = *from++;
                    case 2:
                        *to++ = *from++;
                    case 1:
                        *to++ = *from++;
                } while (--n > 0);
        }
    }

    ClassFileStream &ClassFileStream::operator>>(CONSTANT_Utf8_info &info) {
        info.tag = get_u1();
        info.length = get_u2();
        info.bytes = new u1[info.length];
        get_u1_bytes(info.bytes, info.length);
        return *this;
    }

    ClassFileStream &ClassFileStream::operator>>(CONSTANT_Class_info &info) {
        info.tag = get_u1();
        info.name_index = get_u2();
        return *this;
    }

    ClassFileStream &ClassFileStream::operator>>(CONSTANT_Double_info &info) {
        info.tag = get_u1();
        info.high_bytes = get_u4();
        info.low_bytes = get_u4();
        return *this;
    }

    ClassFileStream &ClassFileStream::operator>>(CONSTANT_Float_info &info) {
        info.tag = get_u1();
        info.bytes = get_u4();
        return *this;
    }

    ClassFileStream &ClassFileStream::operator>>(CONSTANT_Long_info &info) {
        info.tag = get_u1();
        info.high_bytes = get_u4();
        info.low_bytes = get_u4();
        return *this;
    }

    ClassFileStream &ClassFileStream::operator>>(CONSTANT_Fieldref_info &info) {
        info.tag = get_u1();
        info.class_index = get_u2();
        info.name_and_type_index = get_u2();
        return *this;
    }

    ClassFileStream &ClassFileStream::operator>>(CONSTANT_Integer_info &info) {
        info.tag = get_u1();
        info.bytes = get_u4();
        return *this;
    }

    ClassFileStream &ClassFileStream::operator>>(CONSTANT_String_info &info) {
        info.tag = get_u1();
        info.string_index = get_u2();
        return *this;
    }

    ClassFileStream &ClassFileStream::operator>>(CONSTANT_Methodref_info &info) {
        info.tag = get_u1();
        info.class_index = get_u2();
        info.name_and_type_index = get_u2();
        return *this;
    }

    ClassFileStream &ClassFileStream::operator>>(CONSTANT_InterfaceMethodref_info &info) {
        info.tag = get_u1();
        info.class_index = get_u2();
        info.name_and_type_index = get_u2();
        return *this;
    }

    ClassFileStream &ClassFileStream::operator>>(CONSTANT_MethodHandle_info &info) {
        info.tag = get_u1();
        info.reference_kind = get_u1();
        info.reference_index = get_u2();
        return *this;
    }

    ClassFileStream &ClassFileStream::operator>>(CONSTANT_MethodType_info &info) {
        info.tag = get_u1();
        info.descriptor_index = get_u2();
        return *this;
    }

    ClassFileStream &ClassFileStream::operator>>(CONSTANT_NameAndType_info &info) {
        info.tag = get_u1();
        info.name_index = get_u2();
        info.descriptor_index = get_u2();
        return *this;
    }

    ClassFileStream &ClassFileStream::operator>>(CONSTANT_InvokeDynamic_info &info) {
        info.tag = get_u1();
        info.bootstrap_method_attr_index = get_u2();
        info.name_and_type_index = get_u2();
        return *this;
    }
}

#pragma clang diagnostic pop