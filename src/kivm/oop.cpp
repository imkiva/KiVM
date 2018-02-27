//
// Created by kiva on 2018/2/25.
//

#include <kivm/oop.h>

namespace kivm {
    markOopDesc::markOopDesc(oopType type) {
        this->_type = type;
    }

    oopDesc::oopDesc(Klass *klass, oopType type) {
        this->_klass = klass;
        this->_mark = new markOopDesc(type);
    }

    oopDesc::~oopDesc() {
        delete this->_mark;
    }
}
