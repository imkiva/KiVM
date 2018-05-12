//
// Created by kiva on 2018/5/12.
//
#include <kivm/kivm.h>
#include <tuple>
#include <ffi.h>

using namespace kivm;

//union OffsetEncoder {
//    struct {
//        int isStatic;
//        int offset;
//    };
//    jlong result;
//
//    std::tuple<int, bool> decode() {
//        return std::make_tuple(offset, isStatic != 0);
//    };
//
//    OffsetEncoder(int offset, bool isStatic) {
//        this->offset = offset;
//        this->isStatic = isStatic;
//    }
//
//    OffsetEncoder(jlong encoded) {
//        this->result = encoded;
//    }
//};

static jlong encodeOffset(int offset, bool isStatic) {
    ++offset;
//    return OffsetEncoder(offset, isStatic).result;
    if (isStatic) {
        return offset * 2;
    }
    return offset * 2 + 1;
}

static std::tuple<int, bool> decodeOffset(jlong encoded) {
//    return OffsetEncoder(encoded).decode();
    if (encoded % 2 == 0) {
        return std::make_tuple((int) (encoded / 2) - 1, true);
    }
    return std::make_tuple((int) ((encoded - 1) / 2) - 1, false);
};

void test(int offset, bool isStatic) {
    auto encoded = encodeOffset(offset, isStatic);
    int decodedOffset;
    bool decodedIsStatic;
    std::tie(decodedOffset, decodedIsStatic) = decodeOffset(encoded);
    if (decodedOffset != offset
        || decodedIsStatic != isStatic) {
        printf("expected: offset = %d, isStatic = %s, but got offset = %d, isStatic = %s\n",
            offset, isStatic ? "true" : "false",
            decodedOffset, decodedIsStatic ? "true" : "false");
        assert(false);
    }
}

int main() {
    for (int i = 0; i < INT16_MAX; ++i) {
        test(i, true);
        test(i, false);
    }
}
