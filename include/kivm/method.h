//
// Created by kiva on 2018/2/27.
//

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

    public:
        Method(InstanceKlass *clazz, method_info *method_info);

        void link_and_init(cp_info **pool);

        InstanceKlass *get_class() const {
            return _klass;
        }

        const String &get_name() const {
            return _name;
        }

        const String &get_descriptor() const {
            return _descriptor;
        }

        u2 get_access_flag() const {
            return _access_flag;
        }

        bool is_linked() const {
            return _linked;
        }
    };

    class MethodPool {
    private:
        static std::list<Method *> &entries_internal();

    public:
        static void add(Method *method);

        static const std::list<Method *> &entries();
    };
}
