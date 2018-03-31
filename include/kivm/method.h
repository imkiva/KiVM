//
// Created by kiva on 2018/2/27.
//
#pragma once

#include <kivm/kivm.h>
#include <kivm/bytecode/codeBlob.h>
#include <kivm/classfile/attributeInfo.h>
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
        static bool isSame(const Method *lhs, const Method *rhs);

        static String makeIdentity(const Method *m);

    private:
        InstanceKlass *_klass;
        String _name;
        String _descriptor;
        String _signature;
        u2 _access_flag;

        CodeBlob _code_blob;

        method_info *_method_info;
        Exceptions_attribute *_exception_attr;
        Code_attribute *_code_attr;

        /** this method is likely to throw these exceptions **/
        std::list<InstanceKlass *> _throws;

        /** map<start-pc, line-number> **/
        std::unordered_map<u2, u2> _line_number_table;

        bool _linked;

        void linkAttributes(cp_info **pool);

        void linkExceptionAttribute(cp_info **pool, Exceptions_attribute *attr);

        void linkCodeAttribute(cp_info **pool, Code_attribute *attr);

        bool isPcCorrect(u4 pc);

    public:
        Method(InstanceKlass *clazz, method_info *method_info);

        void linkMethod(cp_info **pool);

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

        u2 getAccessFlag() const {
            return _access_flag;
        }

        const CodeBlob &getCodeBlob() const {
            return _code_blob;
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

        bool isSynchronized() const {
            return (getAccessFlag() & ACC_SYNCHRONIZED) == ACC_SYNCHRONIZED;
        }

        bool isFinal() const {
            return (getAccessFlag() & ACC_FINAL) == ACC_FINAL;
        }

        bool isStatic() {
            return (getAccessFlag() & ACC_STATIC) == ACC_STATIC;
        }

        bool isAbstract() {
            return (getAccessFlag() & ACC_ABSTRACT) == ACC_ABSTRACT;
        }

        bool isNative() const {
            return (getAccessFlag() & ACC_NATIVE) == ACC_NATIVE;
        }

        int getMaxLocals() const {
            return _code_attr != nullptr ? _code_attr->max_locals : 0;
        }

        int getMaxStack() const {
            return _code_attr != nullptr ? _code_attr->max_stack : 0;
        }
    };

    /**
     * The implementation of Method Area
     */
    class MethodPool {
    private:
        static std::list<Method *> &getEntriesInternal();

    public:
        static void add(Method *method);

        static const std::list<Method *> &getEntries();
    };
}
