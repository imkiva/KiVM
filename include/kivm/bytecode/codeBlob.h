//
// Created by kiva on 2018/3/30.
//
#pragma once

#include <kivm/kivm.h>

namespace kivm {
    class CodeBlob final {
        friend class Method;

    private:
        u1 *_base;
        u4 _size;

        void init(u1 *base, u4 size) {
            this->_base = base;
            this->_size = size;
        }

    public:
        CodeBlob() : _base(nullptr), _size(0) {}

        inline bool validate() {
            return _base != nullptr && _size > 0;
        }

        inline bool validateOffset(int offset) const {
            return offset >= 0 && offset < _size;
        }

        inline u1 operator[](int offset) const {
            return *(_base + offset);
        }
    };
}
