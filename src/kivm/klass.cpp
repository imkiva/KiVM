//
// Created by kiva on 2018/2/27.
//

#include <kivm/klass.h>
#include <cassert>

namespace kivm {
    Klass::Klass() {
        this->_state = ClassState::allocated;
    }
}
