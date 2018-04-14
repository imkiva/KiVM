//
// Created by kiva on 2018/2/28.
//

#include <kivm/oop/reflectionSupport.h>
#include <kivm/oop/instanceOop.h>
#include <kivm/oop/helper.h>

namespace kivm {

    instanceOopDesc::instanceOopDesc(InstanceKlass *klass)
        : oopDesc(klass, oopType::INSTANCE_OOP) {
        this->_instanceFieldValues.resize(klass->_instanceFields.size());
        for (auto &e : klass->_instanceFields) {
            helperInitField(this->_instanceFieldValues, e.second->_offset, e.second->_field);
        }
    }
}
