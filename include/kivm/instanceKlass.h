//
// Created by kiva on 2018/2/25.
//
#pragma once

#include <kivm/klass.h>

namespace kivm {
    class oopDesc;

    typedef oopDesc *oop;

    class Method;

    class InstanceKlass : public Klass {
    private:
        ClassLoader *_class_loader;

    private:
        ClassFile *_class_file;

        std::unordered_map<u2, InstanceKlass *> _interfaces;
        std::unordered_map<u2, oop> _static_fields;
        std::unordered_map<u2, Method *> _methods;

        void link_constant_pool(cp_info **constant_pool);

        void link_super_class(cp_info **pool);

        void link_methods(cp_info **pool);

        void link_fields(cp_info **pool);

        void link_attributes(cp_info **pool);

    public:
        InstanceKlass(ClassFile *classFile, ClassLoader *class_loader);

        ClassLoader *get_class_loader() const {
            return _class_loader;
        }

        void link_class();
    };
}
