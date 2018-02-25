//
// Created by kiva on 2018/2/25.
//

#include <kivm/classFile.h>
#include <cassert>

namespace kivm {

    ClassFileStream::ClassFileStream(u1 *buffer, int length, char *source) {
        this->_buffer_start = buffer;
        this->_buffer_end = buffer + length;
        this->_source = source;
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
}
