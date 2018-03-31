//
// Created by kiva on 2018/2/28.
//
#pragma once

#include <kivm/kivm.h>
#include <kivm/oop/oopfwd.h>
#include <list>

namespace kivm {
    class Klass;

    class InstanceKlass;

    class field_info;

    class cp_info;

    class ConstantValue_attribute;

    class Field {
        friend InstanceKlass;

    public:
        static bool isSame(const Field *lhs, const Field *rhs);

        static String makeIdentity(InstanceKlass *belongTo, const Field *f);

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

        void linkAttributes(cp_info **pool);

        void linkValueType();

    public:
        Field(InstanceKlass *clazz, field_info *field_info);

        void linkField(cp_info **pool);

        InstanceKlass *getClass() const {
            return _klass;
        }

        const String &getName() const {
            return _name;
        }

        const String &getDescriptor() const {
            return _descriptor;
        }

        const String &getSignature() const {
            return _signature;
        }

        ConstantValue_attribute *getConstantAttribute() const {
            return _constant_attribute;
        }

        u2 getAccessFlag() const {
            return _access_flag;
        }

        ValueType getValueType() const {
            return _value_type;
        }

        bool isLinked() const {
            return _linked;
        }

        bool isPublic() const {
            return (getAccessFlag() & ACC_PUBLIC) == ACC_PUBLIC;
        }

        bool isPrivate() const {
            return (getAccessFlag() & ACC_PRIVATE) == ACC_PRIVATE;
        }

        bool isProtected() const {
            return (getAccessFlag() & ACC_PROTECTED) == ACC_PROTECTED;
        }

        bool isFinal() const {
            return (getAccessFlag() & ACC_FINAL) == ACC_FINAL;
        }

        bool isStatic() {
            return (getAccessFlag() & ACC_STATIC) == ACC_STATIC;
        }

        bool isVolatile() const {
            return (getAccessFlag() & ACC_VOLATILE) == ACC_VOLATILE;
        }
    };

    class FieldPool {
    private:
        static std::list<Field *> &getEntriesInternal();

    public:
        static void add(Field *method);

        static const std::list<Field *> &getEntries();
    };
}
