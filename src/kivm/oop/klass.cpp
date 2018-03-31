//
// Created by kiva on 2018/2/27.
//

#include <kivm/oop/klass.h>

namespace kivm {
    Klass::Klass() {
        setClassState(ClassState::ALLOCATED);
    }
}
