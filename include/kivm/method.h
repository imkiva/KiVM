//
// Created by kiva on 2018/2/27.
//
#pragma once

#include <kivm/kivm.h>
#include <list>
#include <unordered_map>

namespace kivm {
    class InstanceKlass;

    class method_info;

    class cp_info;

    class Code_attribute;

    class Exceptions_attribute;

    class Method {
    public:
        static bool is_same(const Method *lhs, const Method *rhs);

        static const String &make_identity(const Method *m);

    private:
        InstanceKlass *_klass;
        String _name;
        String _descriptor;
        String _signature;
        u2 _access_flag;

        method_info *_method_info;
        Exceptions_attribute *_exception_attr;
        Code_attribute *_code_attr;

        /** this method is likely to throw these exceptions **/
        std::list<InstanceKlass *> _throws;

        /** map<start-pc, line-number> **/
        std::unordered_map<u2, u2> _line_number_table;

        bool _linked;

        void link_attributes(cp_info **pool);

        void link_exception_attribute(cp_info **pool, Exceptions_attribute *attr);

        void link_code_attribute(cp_info **pool, Code_attribute *attr);

        bool is_pc_in_method(const u1 *pc);

    public:
        Method(InstanceKlass *clazz, method_info *method_info);

        void link_method(cp_info **pool);

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

        bool is_synchronized() const {
            return (get_access_flag() & ACC_SYNCHRONIZED) == ACC_SYNCHRONIZED;
        }

        bool is_final() const {
            return (get_access_flag() & ACC_FINAL) == ACC_FINAL;
        }

        bool is_static() {
            return (get_access_flag() & ACC_STATIC) == ACC_STATIC;
        }

        bool is_abstract() {
            return (get_access_flag() & ACC_ABSTRACT) == ACC_ABSTRACT;
        }

        bool is_native() const {
            return (get_access_flag() & ACC_NATIVE) == ACC_NATIVE;
        }
    };

    /**
     * The implementation of Method Area
     */
    class MethodPool {
    private:
        static std::list<Method *> &entries_internal();

    public:
        static void add(Method *method);

        static const std::list<Method *> &entries();
    };
}
