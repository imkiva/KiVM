//
// Created by kiva on 2018/5/12.
//
#include <kivm/kivm.h>
#include <tuple>
#include <ffi.h>

using namespace kivm;

union OffsetEncoder {
    struct {
        int isStatic;
        int offset;
    };
    jlong result;

    std::tuple<int, bool> decode() {
        return std::make_tuple(offset, isStatic != 0);
    };

    OffsetEncoder(int offset, bool isStatic) {
        this->offset = offset;
        this->isStatic = isStatic;
    }

    OffsetEncoder(jlong encoded) {
        this->result = encoded;
    }
};

static jlong encodeOffset(int offset, bool isStatic) {
    return OffsetEncoder(offset, isStatic).result;
}

static std::tuple<int, bool> decodeOffset(jlong encoded) {
    return OffsetEncoder(encoded).decode();
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

    int offset;
    bool isStatic;
    std::tie(offset, isStatic) = decodeOffset(0);
    assert(offset >= 0);
}
