//
// Created by kiva on 2018/2/25.
//

#include <kivm/oop.h>

template<typename T, typename...ArgsT>
void construct_object(T *ptr, ArgsT &&...args) {
    if (ptr != nullptr) {
        ::new(ptr) T(std::forward<ArgsT>(args)...);
    }
}

oop oopDesc::copy() {
    auto _oop = static_cast<oop>(oopBase::operator new(sizeof(*this), false));
    construct_object(_oop, this);
    return _oop;
}

oopDesc::~oopDesc() = default;

