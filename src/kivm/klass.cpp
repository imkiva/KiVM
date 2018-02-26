//
// Created by kiva on 2018/2/27.
//

#include <kivm/klass.h>
#include <cassert>

namespace kivm {
    template<typename T>
    struct constant_helper {
        constexpr static u2 get_tag() = delete;
    };

    template<>
    struct constant_helper<CONSTANT_Utf8_info> {
        constexpr static u2 get_tag() { return CONSTANT_Utf8; }
    };

    template<>
    struct constant_helper<CONSTANT_Integer_info> {
        constexpr static u2 get_tag() { return CONSTANT_Integer; }
    };

    template<>
    struct constant_helper<CONSTANT_Float_info> {
        constexpr static u2 get_tag() { return CONSTANT_Float; }
    };

    template<>
    struct constant_helper<CONSTANT_Long_info> {
        constexpr static u2 get_tag() { return CONSTANT_Long; }
    };

    template<>
    struct constant_helper<CONSTANT_Double_info> {
        constexpr static u2 get_tag() { return CONSTANT_Double; }
    };

    template<>
    struct constant_helper<CONSTANT_Class_info> {
        constexpr static u2 get_tag() { return CONSTANT_Class; }
    };

    template<>
    struct constant_helper<CONSTANT_String_info> {
        constexpr static u2 get_tag() { return CONSTANT_String; }
    };

    template<>
    struct constant_helper<CONSTANT_Fieldref_info> {
        constexpr static u2 get_tag() { return CONSTANT_Fieldref; }
    };

    template<>
    struct constant_helper<CONSTANT_Methodref_info> {
        constexpr static u2 get_tag() { return CONSTANT_Methodref; }
    };

    template<>
    struct constant_helper<CONSTANT_InterfaceMethodref_info> {
        constexpr static u2 get_tag() { return CONSTANT_InterfaceMethodref; }
    };

    template<>
    struct constant_helper<CONSTANT_NameAndType_info> {
        constexpr static u2 get_tag() { return CONSTANT_NameAndType; }
    };

    template<>
    struct constant_helper<CONSTANT_MethodHandle_info> {
        constexpr static u2 get_tag() { return CONSTANT_MethodHandle; }
    };

    template<>
    struct constant_helper<CONSTANT_MethodType_info> {
        constexpr static u2 get_tag() { return CONSTANT_MethodType; }
    };

    template<>
    struct constant_helper<CONSTANT_InvokeDynamic_info> {
        constexpr static u2 get_tag() { return CONSTANT_InvokeDynamic; }
    };

    template<typename T>
    T *require_constant(cp_info **pool, u2 index) {
        cp_info *info = pool[index];
        if (info->tag != constant_helper<T>::get_tag()) {
            // TODO: throw VerifyError
            assert(false);
            return nullptr;
        }
        return (T *) info;
    }

    Klass *require_class(ClassLoader *classLoader, const String &class_name) {
        if (classLoader == nullptr) {
            // This is a bootstrap class
            classLoader = BootstrapClassLoader::get_class_loader();
        }

        Klass *loaded_class = classLoader == nullptr
                              ? nullptr
                              : classLoader->loadClass(class_name);
        if (loaded_class == nullptr) {
            // TODO: throw LinkageError
            assert(false);
        }
        return loaded_class;
    }

    Klass::Klass() {
        this->_state = ClassState::allocated;
    }

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
            // java.lang.Object needs no superclass.
            return;
        }
        auto *class_info = require_constant<CONSTANT_Class_info>(pool, _class_file->super_class);
        auto *utf8_info = require_constant<CONSTANT_Utf8_info>(pool, class_info->name_index);
        set_super_class(require_class(get_class_loader(), utf8_info->get_constant()));
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
