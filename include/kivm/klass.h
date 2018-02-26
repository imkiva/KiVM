//
// Created by kiva on 2018/2/25.
//
#pragma once

#include <unordered_map>
#include <kivm/kivm.h>
#include <kivm/classFile.h>
#include <kivm/classLoader.h>

namespace kivm {
    enum ClassType {
        INSTANCE_CLASS,
        OBJECT_ARRAY_CLASS,
        TYPE_ARRAY_CLASS,
    };

    enum oopType {
        INSTANCE_OOP,
        PRIMITIVE_OOP,
        OBJECT_ARRAY_OOP,
        TYPE_ARRAY_OOP,
    };

    enum ClassState {
        allocated,
        loaded,
        linked,
        being_initialized,
        fully_initialized,
        initialization_error,
    };

    class Klass {
    private:
        ClassState _state;
        u2 _access_flag;

    protected:
        String _name;
        ClassType _type;

        Klass *_super_class;

    public:
        ClassState get_state() const {
            return _state;
        }

        void set_state(ClassState _state) {
            this->_state = _state;
        }

        u2 get_access_flag() const {
            return _access_flag;
        }

        void set_access_flag(u2 _access_flag) {
            Klass::_access_flag = _access_flag;
        }

        const String &get_name() const {
            return _name;
        }

        void set_name(const String &_name) {
            this->_name = _name;
        }

        ClassType get_type() const {
            return _type;
        }

        void set_type(ClassType _type) {
            this->_type = _type;
        }

        Klass *get_super_class() const {
            return _super_class;
        }

        void set_super_class(Klass *_super_class) {
            this->_super_class = _super_class;
        }

    public:
        Klass();

        virtual ~Klass() = default;
    };

    class oopDesc;

    typedef oopDesc *oop;

    class InstanceKlass : public Klass {
    private:
        ClassLoader *_class_loader;

    private:
        ClassFile *_class_file;

        std::unordered_map<u2, InstanceKlass *> _interfaces;
        std::unordered_map<u2, oop> _static_fields;

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

