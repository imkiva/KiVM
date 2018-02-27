//
// Created by kiva on 2018/2/28.
//
#pragma once

#include <kivm/kivm.h>
#include <list>

namespace kivm {
    class Klass;

    class InstanceKlass;

    class field_info;

    class cp_info;

    class ConstantValue_attribute;

    enum ValueType {
        BYTE,
        BOOLEAN,
        CHAR,
        SHORT,
        INT,
        FLOAT,
        LONG,
        DOUBLE,
        OBJECT,
        ARRAY,
    };

    class Field {
    public:
        static bool is_same(const Field *lhs, const Field *rhs);

        static String make_identity(const Field *f);

    private:
        InstanceKlass *_klass;
        String _name;
        String _descriptor;
        String _signature;
        u2 _access_flag;

        ValueType _value_type;

        /**
         * Only available when _value_type is OBJECT or ARRAY
         */
        Klass *_value_class_type;

        field_info *_field_info;
        ConstantValue_attribute *_constant_attribute;

        bool _linked;

        void link_attributes(cp_info **pool);

        void link_value_type();

    public:
        Field(InstanceKlass *clazz, field_info *field_info);

        void link_field(cp_info **pool);

        InstanceKlass *get_class() const {
            return _klass;
        }

        const String &get_name() const {
            return _name;
        }

        const String &get_descriptor() const {
            return _descriptor;
        }

        const String &get_signature() const {
            return _signature;
        }

        u2 get_access_flag() const {
            return _access_flag;
        }

        ValueType get_value_type() const {
            return _value_type;
        }

        bool is_linked() const {
            return _linked;
        }

        bool is_public() const {
            return (get_access_flag() & ACC_PUBLIC) == ACC_PUBLIC;
        }

        bool is_private() const {
            return (get_access_flag() & ACC_PRIVATE) == ACC_PRIVATE;
        }

        bool is_protected() const {
            return (get_access_flag() & ACC_PROTECTED) == ACC_PROTECTED;
        }

        bool is_final() const {
            return (get_access_flag() & ACC_FINAL) == ACC_FINAL;
        }

        bool is_static() {
            return (get_access_flag() & ACC_STATIC) == ACC_STATIC;
        }

        bool is_volatile() const {
            return (get_access_flag() & ACC_VOLATILE) == ACC_VOLATILE;
        }
    };

    class FieldPool {
    private:
        static std::list<Field *> &entries_internal();

    public:
        static void add(Field *method);

        static const std::list<Field *> &entries();
    };
}
