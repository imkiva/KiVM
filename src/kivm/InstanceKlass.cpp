//
// Created by kiva on 2018/2/27.
//

#include <kivm/instanceKlass.h>
#include <kivm/method.h>
#include <sstream>

namespace kivm {
    InstanceKlass::InstanceKlass(ClassFile *classFile, ClassLoader *_class_loader) {
        this->set_type(ClassType::INSTANCE_CLASS);
        this->_class_file = classFile;
        this->_class_loader = _class_loader;
    }

    InstanceKlass *InstanceKlass::require_instance_class(u2 class_info_index) {
        auto *class_info = require_constant<CONSTANT_Class_info>(_class_file->constant_pool,
                                                                 class_info_index);
        auto *utf8_info = require_constant<CONSTANT_Utf8_info>(_class_file->constant_pool,
                                                               class_info->name_index);

        Klass *loaded = ClassLoader::require_class(get_class_loader(), utf8_info->get_constant());
        if (loaded->get_type() != ClassType::INSTANCE_CLASS) {
            // TODO: throw VerifyError
            assert(false);
            return nullptr;
        }
        return (InstanceKlass *) loaded;
    }

    void InstanceKlass::link_and_init() {
        cp_info **pool = _class_file->constant_pool;
        this->set_access_flag(_class_file->access_flags);

        auto *class_info = require_constant<CONSTANT_Class_info>(pool, _class_file->this_class);
        auto *utf8_info = require_constant<CONSTANT_Utf8_info>(pool, class_info->name_index);
        this->set_name(utf8_info->get_constant());

        link_super_class(pool);
        link_interfaces(pool);
        link_methods(pool);
        link_fields(pool);
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
            set_super_class(nullptr);
            return;
        }

        auto *super_class = require_instance_class(_class_file->super_class);
        if (super_class->is_final()) {
            // TODO: throw VerifyError
            assert(false);
        }
        set_super_class(super_class);
    }

    void InstanceKlass::link_interfaces(cp_info **pool) {
        for (int i = 0; i < _class_file->interfaces_count; ++i) {
            InstanceKlass *interface_class = require_instance_class(_class_file->interfaces[i]);
            _interfaces.insert(std::make_pair(interface_class->get_name(), interface_class));
        }
    }

    void InstanceKlass::link_methods(cp_info **pool) {
        // for a easy implementation, I just copy superclass's vtable.
        if (get_super_class() != nullptr) {
            auto *sc = (InstanceKlass *) get_super_class();
            this->_vtable = sc->_vtable;
        }

        for (int i = 0; i < _class_file->methods_count; ++i) {
            auto *method = new Method(this, _class_file->methods + i);
            method->link_method(pool);
            MethodPool::add(method);

            if (method->is_final() || method->is_private()) {
                _pftable.insert(std::make_pair(Method::make_identity(method), method));

            } else if (method->is_static()) {
                _stable.insert(std::make_pair(Method::make_identity(method), method));

            } else {
                // Do not use _vtable.insert()
                // This may override superclass's virtual methods.
                _vtable[Method::make_identity(method)] = method;
            }
        }
    }

    void InstanceKlass::link_fields(cp_info **pool) {

    }

    void InstanceKlass::link_constant_pool(cp_info **constant_pool) {
    }

    void InstanceKlass::link_attributes(cp_info **pool) {
    }
}
