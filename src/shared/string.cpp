//
// Created by kiva on 2018/2/25.
//

#include <shared/string.h>
#include <cassert>
#include <vector>
#include <codecvt>
#include <locale>

namespace kivm {
    namespace strings {
        /* Copied from: https://github.com/wind2412/wind_jvm/blob/master/src/class_parser.cpp */
        constexpr int BIT_NUM = 8;

        static bool test_bit_1(const u1 *bytes, size_t length,
                               int position, int bit_pos) {
            u1 target = bytes[position];
            int real_pos = BIT_NUM - position - 1;
            assert(real_pos >= 0);
            return ((target >> real_pos) & 1) == 1;
        }

        // $ 4.4.7
        static bool is_first_type(const u1 *bytes, size_t length, int position) {
            assert(position + 1 <= length);
            return (bytes[position] & 0x80) == 0;
        }

        static bool is_second_type(const u1 *bytes, size_t length, int position) {
            assert(position + 2 <= length);
            return (bytes[position] & 0xE0) == 0xC0
                   && (bytes[position + 1] & 0xC0) == 0x80;
        }

        static bool is_third_type(const u1 *bytes, size_t length, int position) {
            assert(position + 3 <= length);
            return (bytes[position] & 0xF0) == 0xE0
                   && (bytes[position + 1] & 0xC0) == 0x80
                   && (bytes[position + 2] & 0xC0) == 0x80;
        }

        static bool is_forth_type(const u1 *bytes, size_t length, int position) {
            assert(position + 6 <= length);
            return (bytes[position] == 0xED)
                   && (bytes[position + 1] & 0xF0) == 0xA0
                   && (bytes[position + 2] & 0xC0) == 0x80
                   && (bytes[position + 3] == 0xED)
                   && (bytes[position + 4] & 0xF0) == 0xB0
                   && (bytes[position + 5] & 0xC0) == 0x80;
        }

        static u2 get_first_type(const u1 *bytes, size_t length, int position) {
            return bytes[position];
        }

        static u2 get_second_type(const u1 *bytes, size_t length, int position) {
            return static_cast<u2>(((bytes[position] & 0x1f) << 6)
                                   + (bytes[position + 1] & 0x3f));
        }

        static u2 get_third_type(const u1 *bytes, size_t length, int position) {
            return static_cast<u2>(((bytes[position] & 0xf) << 12)
                                   + ((bytes[position + 1] & 0x3f) << 6)
                                   + (bytes[position + 2] & 0x3f));
        }

        static u2 get_forth_type(const u1 *bytes, size_t length, int position) {
            return static_cast<u2>(0x10000
                                   + ((bytes[position + 1] & 0x0f) << 16)
                                   + ((bytes[position + 2] & 0x3f) << 10)
                                   + ((bytes[position + 4] & 0x0f) << 6)
                                   + (bytes[position + 5] & 0x3f));
        }

        String from_bytes(u1 *bytes, size_t length) {
            std::vector<u2> buffer;

            for (int pos = 0; pos < length;) {
                if (is_first_type(bytes, length, pos)) {
                    buffer.push_back(get_first_type(bytes, length, pos));
                    pos += 1;

                } else if (is_second_type(bytes, length, pos)) {
                    buffer.push_back(get_second_type(bytes, length, pos));
                    pos += 2;

                } else if (is_third_type(bytes, length, pos)) {
                    buffer.push_back(get_third_type(bytes, length, pos));
                    pos += 3;

                } else if (is_forth_type(bytes, length, pos)) {
                    buffer.push_back(get_forth_type(bytes, length, pos));
                    pos += 6;

                } else {
                    // should never reach here
                    assert(false);
                }
            }
            return String(buffer.begin(), buffer.end());
        }

        String from_std_string(const std::string &str) {
            std::wstring_convert<std::codecvt_utf8<wchar_t >> convert;
            return convert.from_bytes(str);
        }

        std::string to_std_string(const String &str) {
            std::wstring_convert<std::codecvt_utf8<wchar_t>> convert;
            return convert.to_bytes(str);
        }
    }
}
