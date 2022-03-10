//
// Created by kiva on 2018/2/25.
//

#include <shared/string.h>
#include <cassert>
#include <vector>
#include <codecvt>
#include <locale>
#include <sstream>
#include <utility>

namespace kivm {
    namespace strings {
        static std::wstring_convert<std::codecvt_utf8<wchar_t>> CONVERT;

        // $ 4.4.7
        static bool isFirstType(const u1 *bytes, size_t length, int position) {
            assert(position + 1 <= length);
            return (bytes[position] & 0x80) == 0;
        }

        static bool isSecondType(const u1 *bytes, size_t length, int position) {
            assert(position + 2 <= length);
            return (bytes[position] & 0xE0) == 0xC0
                   && (bytes[position + 1] & 0xC0) == 0x80;
        }

        static bool isThirdType(const u1 *bytes, size_t length, int position) {
            assert(position + 3 <= length);
            return (bytes[position] & 0xF0) == 0xE0
                   && (bytes[position + 1] & 0xC0) == 0x80
                   && (bytes[position + 2] & 0xC0) == 0x80;
        }

        static bool isForthType(const u1 *bytes, size_t length, int position) {
            assert(position + 6 <= length);
            return (bytes[position] == 0xED)
                   && (bytes[position + 1] & 0xF0) == 0xA0
                   && (bytes[position + 2] & 0xC0) == 0x80
                   && (bytes[position + 3] == 0xED)
                   && (bytes[position + 4] & 0xF0) == 0xB0
                   && (bytes[position + 5] & 0xC0) == 0x80;
        }

        static u2 getFirstType(const u1 *bytes, size_t length, int position) {
            return bytes[position];
        }

        static u2 getSecondType(const u1 *bytes, size_t length, int position) {
            return static_cast<u2>(((bytes[position] & 0x1f) << 6)
                                   + (bytes[position + 1] & 0x3f));
        }

        static u2 getThirdType(const u1 *bytes, size_t length, int position) {
            return static_cast<u2>(((bytes[position] & 0xf) << 12)
                                   + ((bytes[position + 1] & 0x3f) << 6)
                                   + (bytes[position + 2] & 0x3f));
        }

        static u2 getForthType(const u1 *bytes, size_t length, int position) {
            return static_cast<u2>(0x10000
                                   + ((bytes[position + 1] & 0x0f) << 16)
                                   + ((bytes[position + 2] & 0x3f) << 10)
                                   + ((bytes[position + 4] & 0x0f) << 6)
                                   + (bytes[position + 5] & 0x3f));
        }

        String fromBytes(u1 *bytes, size_t length) {
            std::vector<u2> buffer;

            for (int pos = 0; pos < length;) {
                if (isFirstType(bytes, length, pos)) {
                    buffer.push_back(getFirstType(bytes, length, pos));
                    pos += 1;

                } else if (isSecondType(bytes, length, pos)) {
                    buffer.push_back(getSecondType(bytes, length, pos));
                    pos += 2;

                } else if (isThirdType(bytes, length, pos)) {
                    buffer.push_back(getThirdType(bytes, length, pos));
                    pos += 3;

                } else if (isForthType(bytes, length, pos)) {
                    buffer.push_back(getForthType(bytes, length, pos));
                    pos += 6;

                } else {
                    // should never reach here
                    assert(false);
                }
            }
            return String(buffer.begin(), buffer.end());
        }

        String fromStdString(const std::string &str) {
            return CONVERT.from_bytes(str);
        }

        std::string toStdString(const String &str) {
            return CONVERT.to_bytes(str);
        }

        String replaceAll(const String &string, const String &oldValue, const String &newValue) {
            auto oldSize = oldValue.size();
            auto newSize = newValue.size();

            String newString = string;
            String::size_type pos = 0;
            while ((pos = newString.find(oldValue, pos)) != String::npos) {
                newString.replace(pos, oldSize, newValue);
                pos += newSize;
            }
            return newString;
        }

        std::vector<String> split(const String &string, const String &delimiter) {
            size_t start = 0;
            size_t end;
            size_t delimiterLength = delimiter.length();
            std::vector<String> results;

            while ((end = string.find(delimiter, start)) != String::npos) {
                results.push_back(string.substr(start, end - start));
                start = end + delimiterLength;
            }

            results.push_back(string.substr(start));
            return results;
        }
    }
}
