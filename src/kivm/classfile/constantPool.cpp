//
// Created by kiva on 2018/2/25.
//

#include <kivm/classfile/constantPool.h>
#include <cmath>

namespace kivm {
    jint CONSTANT_Integer_info::get_constant() const {
        return static_cast<jint>(bytes);
    }

    jfloat CONSTANT_Float_info::get_constant() const {
        if (FLOAT_IS_POSITIVE_INFINITY(bytes)) {
            return _FLOAT_POSITIVE_INFINITY;

        } else if (FLOAT_IS_NEGATIVE_INFINITY(bytes)) {
            return _FLOAT_NEGATIVE_INFINITY;

        } else if (FLOAT_IS_NAN(bytes)) {
            return FLOAT_NAN;

        } else {
            int s = ((bytes >> 31) == 0) ? 1 : -1;
            int e = ((bytes >> 23) & 0xff);
            int m = (e == 0)
                    ? (bytes & 0x7fffff) << 1
                    : (bytes & 0x7fffff) | 0x800000;
            return static_cast<jfloat>(s * m * pow(2, e - 150));
        }
    }

    jlong CONSTANT_Long_info::get_constant() const {
        return ((jlong) high_bytes << 32) + low_bytes;
    }

    jdouble CONSTANT_Double_info::get_constant() const {
        jlong bits = ((jlong) high_bytes << 32) + low_bytes;

        if (DOUBLE_IS_POSITIVE_INFINITY(bits)) {
            return _DOUBLE_POSITIVE_INFINITY;

        } else if (DOUBLE_IS_NEGATIVE_INFINITY(bits)) {
            return _DOUBLE_NEGATIVE_INFINITY;

        } else if (DOUBLE_IS_NAN(bits)) {
            return DOUBLE_NAN;

        } else {
            int s = ((bits >> 63) == 0) ? 1 : -1;
            int e = (int) ((bits >> 52) & 0x7ffL);
            long m = (e == 0)
                     ? (bits & 0xfffffffffffffL) << 1
                     : (bits & 0xfffffffffffffL) | 0x10000000000000L;
            return s * m * pow(2, e - 1075);
        }
    }

    String CONSTANT_Utf8_info::get_constant() {
        // UTF-8 Strings in Java needs to be Unicode in C++
        if (!_cached) {
            _cached_string = kivm::strings::fromBytes(bytes, length);
            _cached = true;
        }

        return _cached_string;
    }

    CONSTANT_Utf8_info::CONSTANT_Utf8_info() : cp_info() {
        bytes = nullptr;
        _cached = false;
    }

    CONSTANT_Utf8_info::~CONSTANT_Utf8_info() {
        delete[] bytes;
    }
}
