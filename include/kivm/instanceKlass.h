//
// Created by kiva on 2018/2/25.
//
#pragma once

#include <kivm/klass.h>
#include <unordered_map>
#include <vector>

namespace kivm {
    class oopDesc;

    typedef oopDesc *oop;

    class Method;

    class Field;

    class InstanceKlass : public Klass {
    private:
        ClassLoader *_class_loader;

    private:
        ClassFile *_class_file;

        /**
         * Methods that have been invoked by invokevirtual.
         * map<constant-index, method>
         */
        std::unordered_map<u2, Method *> _vtable_cache;

        /**
         * virtual methods (public or protected methods).
         * map<name + " " + descriptor, method>
         */
        std::unordered_map<String, Method *> _vtable;

        /**
         * private or final methods.
         * map<name + " " + descriptor, method>
         */
        std::unordered_map<String, Method *> _pftable;

        /**
         * static methods.
         * map<name + " " + descriptor, method>
         */
        std::unordered_map<String, Method *> _stable;

        /**
         * static fields.
         * map<name + " " + descriptor, <vector-offset, Field*>>
         */
        std::unordered_map<String, std::pair<u2, Field *>> _static_fields;

        /**
         * instance fields.
         * map<name + " " + descriptor, <vector-offset, Field*>>
         */
        std::unordered_map<String, std::pair<u2, Field *>> _instance_fields;

        /**
         * static fields' values.
         */
        std::vector<oop> _static_field_values;

        /**
         * instance fields' initial values.
         */
        std::vector<oop> _instance_field_init_values;

        /**
         * interfaces
         * map<interface-name, class>
         */
        std::unordered_map<String, InstanceKlass *> _interfaces;

        InstanceKlass *require_instance_class(u2 class_info_index);

        void link_constant_pool(cp_info **constant_pool);

        void link_super_class(cp_info **pool);

        void link_methods(cp_info **pool);

        void link_interfaces(cp_info **pool);

        void link_fields(cp_info **pool);

        void link_attributes(cp_info **pool);

    public:
        InstanceKlass(ClassFile *classFile, ClassLoader *class_loader);

        ClassLoader *get_class_loader() const {
            return _class_loader;
        }

        void link_and_init() override;
    };
}
