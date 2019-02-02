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
        InstanceKlass *_klass = nullptr;
        String _name;
        String _descriptor;
        String _signature;
        u2 _accessFlag;

        ValueType _valueType;

        /**
         * Only available when _value_type is OBJECT or ARRAY
         */
        Klass *_valueClassType = nullptr;
        String _valueClassTypeName;

        field_info *_fieldInfo = nullptr;
        ConstantValue_attribute *_constantAttr = nullptr;

        bool _linked;

        void linkAttributes(cp_info **pool);

        void postLinkValueType();

    public:
        Field(InstanceKlass *clazz, field_info *fieldInfo);

        void linkField(cp_info **pool);

        InstanceKlass *getClass() const {
            return _klass;
        }

        Klass *getValueTypeClass();

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
            return _constantAttr;
        }

        u2 getAccessFlag() const {
            return _accessFlag;
        }

        ValueType getValueType() const {
            return _valueType;
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
