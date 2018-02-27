//
// Created by kiva on 2018/2/27.
//
#include <kivm/instanceKlass.h>

namespace kivm {
    InstanceKlass::InstanceKlass(ClassFile *classFile, ClassLoader *_class_loader) {
        this->set_type(ClassType::INSTANCE_CLASS);
        this->_class_file = classFile;
        this->_class_loader = _class_loader;
    }

    void InstanceKlass::link_class() {
        cp_info **pool = _class_file->constant_pool;

        auto *class_info = require_constant<CONSTANT_Class_info>(pool, _class_file->this_class);
        auto *utf8_info = require_constant<CONSTANT_Utf8_info>(pool, class_info->name_index);

        this->set_name(utf8_info->get_constant());
        this->set_access_flag(_class_file->access_flags);

        link_super_class(pool);
        link_fields(pool);
        link_methods(pool);
        link_constant_pool(pool);
        link_attributes(pool);
        this->set_state(ClassState::linked);
    }

    void InstanceKlass::link_super_class(cp_info **pool) {
        if (_class_file->super_class == 0) {
            // java.lang.Object
            if (get_name() != L"java/lang/Object") {
                // TODO: throw VerifyError
                assert(false);
            }
            // java.lang.Object does not need a superclass.
            return;
        }
        auto *class_info = require_constant<CONSTANT_Class_info>(pool, _class_file->super_class);
        auto *utf8_info = require_constant<CONSTANT_Utf8_info>(pool, class_info->name_index);
        set_super_class(ClassLoader::require_class(get_class_loader(), utf8_info->get_constant()));
    }

    void InstanceKlass::link_fields(cp_info **pool) {

    }

    void InstanceKlass::link_methods(cp_info **pool) {

    }

    void InstanceKlass::link_constant_pool(cp_info **constant_pool) {

    }

    void InstanceKlass::link_attributes(cp_info **pool) {

    }
}
