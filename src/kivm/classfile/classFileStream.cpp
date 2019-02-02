//
// Created by kiva on 2018/2/25.
//

#include <kivm/classfile/classFileStream.h>
#include <cassert>

#pragma clang diagnostic push
#pragma ide diagnostic ignored "missing_default_case"
namespace kivm {
    void ClassFileStream::init(u1 *buffer, size_t length) {
        this->_bufferStart = buffer;
        this->_bufferEnd = buffer + length;
        setNeedVerify(false);
        setCurrent(buffer);
    }

    u1 ClassFileStream::get1() {
        if (_needVerify) {
            guaranteeMore(1);
        } else {
            assert(1 <= _bufferEnd - _current);
        }
        return get1Fast();
    }

    u2 ClassFileStream::get2() {
        if (_needVerify) {
            guaranteeMore(2);
        } else {
            assert(2 <= _bufferEnd - _current);
        }
        return get2Fast();
    }

    u4 ClassFileStream::get4() {
        if (_needVerify) {
            guaranteeMore(4);
        } else {
            assert(4 <= _bufferEnd - _current);
        }
        return get4Fast();
    }

    u8 ClassFileStream::get8() {
        if (_needVerify) {
            guaranteeMore(8);
        } else {
            assert(8 <= _bufferEnd - _current);
        }
        return get8Fast();
    }

    void ClassFileStream::skip1(int length) {
        if (_needVerify) {
            guaranteeMore(length);
        }
        _current += length;
    }

    void ClassFileStream::skip2(int length) {
        if (_needVerify) {
            guaranteeMore(length * 2);
        }
        _current += length * 2;
    }

    void ClassFileStream::skip4(int length) {
        if (_needVerify) {
            guaranteeMore(length * 4);
        }
        _current += length * 4;
    }

    void ClassFileStream::getBytes(u1 *to, int count) {
        u1 *from = asU1Buffer();
        skip1(count);

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
        info.tag = get1();
        info.length = get2();
        info.bytes = new u1[info.length];
        getBytes(info.bytes, info.length);
        return *this;
    }

    ClassFileStream &ClassFileStream::operator>>(CONSTANT_Class_info &info) {
        info.tag = get1();
        info.name_index = get2();
        return *this;
    }

    ClassFileStream &ClassFileStream::operator>>(CONSTANT_Double_info &info) {
        info.tag = get1();
        info.high_bytes = get4();
        info.low_bytes = get4();
        return *this;
    }

    ClassFileStream &ClassFileStream::operator>>(CONSTANT_Float_info &info) {
        info.tag = get1();
        info.bytes = get4();
        return *this;
    }

    ClassFileStream &ClassFileStream::operator>>(CONSTANT_Long_info &info) {
        info.tag = get1();
        info.high_bytes = get4();
        info.low_bytes = get4();
        return *this;
    }

    ClassFileStream &ClassFileStream::operator>>(CONSTANT_Fieldref_info &info) {
        info.tag = get1();
        info.class_index = get2();
        info.name_and_type_index = get2();
        return *this;
    }

    ClassFileStream &ClassFileStream::operator>>(CONSTANT_Integer_info &info) {
        info.tag = get1();
        info.bytes = get4();
        return *this;
    }

    ClassFileStream &ClassFileStream::operator>>(CONSTANT_String_info &info) {
        info.tag = get1();
        info.string_index = get2();
        return *this;
    }

    ClassFileStream &ClassFileStream::operator>>(CONSTANT_Methodref_info &info) {
        info.tag = get1();
        info.class_index = get2();
        info.name_and_type_index = get2();
        return *this;
    }

    ClassFileStream &ClassFileStream::operator>>(CONSTANT_InterfaceMethodref_info &info) {
        info.tag = get1();
        info.class_index = get2();
        info.name_and_type_index = get2();
        return *this;
    }

    ClassFileStream &ClassFileStream::operator>>(CONSTANT_MethodHandle_info &info) {
        info.tag = get1();
        info.reference_kind = get1();
        info.reference_index = get2();
        return *this;
    }

    ClassFileStream &ClassFileStream::operator>>(CONSTANT_MethodType_info &info) {
        info.tag = get1();
        info.descriptor_index = get2();
        return *this;
    }

    ClassFileStream &ClassFileStream::operator>>(CONSTANT_NameAndType_info &info) {
        info.tag = get1();
        info.name_index = get2();
        info.descriptor_index = get2();
        return *this;
    }

    ClassFileStream &ClassFileStream::operator>>(CONSTANT_InvokeDynamic_info &info) {
        info.tag = get1();
        info.bootstrap_method_attr_index = get2();
        info.name_and_type_index = get2();
        return *this;
    }
}

#pragma clang diagnostic pop