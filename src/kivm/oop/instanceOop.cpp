//
// Created by kiva on 2018/2/28.
//

#include <kivm/oop/instanceOop.h>
#include <kivm/oop/helper.h>

namespace kivm {

    instanceOopDesc::instanceOopDesc(InstanceKlass *klass)
            : oopDesc(klass, oopType::INSTANCE_OOP) {
        this->_instance_field_values.resize(klass->_instance_fields.size());
        for (auto &e:klass->_instance_fields) {
            helperInitField(this->_instance_field_values, e.second._field);
        }
    }
}
