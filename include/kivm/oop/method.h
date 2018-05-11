//
// Created by kiva on 2018/2/27.
//
#pragma once

#include <kivm/kivm.h>
#include <kivm/oop/oopfwd.h>
#include <kivm/bytecode/codeBlob.h>
#include <kivm/classfile/attributeInfo.h>
#include <kivm/classfile/annotation.h>
#include <list>
#include <unordered_map>
#include <vector>

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
        u2 _accessFlag;

        /**
         * basic information about a method
         */
        CodeBlob _codeBlob;
        method_info *_methodInfo;
        Exceptions_attribute *_exceptionAttr;
        Code_attribute *_codeAttr;

        /**
         * only available when this method is a native method
         */
        void *_nativePointer;

        /**
         * flags related to descriptor parsing
         */
        bool _argumentValueTypesResolved;
        bool _argumentValueTypesNoWrapResolved;
        bool _returnTypeNoWrapResolved;
        bool _returnTypeResolved;
        bool _linked;

        /**
         * result of descriptor parsing
         */
        ValueType _returnType;
        ValueType _returnTypeNoWrap;
        std::vector<ValueType> _argumentValueTypes;
        std::vector<ValueType> _argumentValueTypesNoWrap;

        /** this method is likely to throw these exceptions **/
        std::list<InstanceKlass *> _throws;

        /** map<start-pc, line-number> **/
        std::unordered_map<u2, u2> _lineNumberTable;

        /**
         * annotations
         */
        ParameterAnnotation *_runtimeVisibleAnnos;
        std::list<ParameterAnnotation *> _runtimeVisibleParameterAnnos;
        std::list<TypeAnnotation *> _runtimeVisibleTypeAnnos;

    private:
        void linkAttributes(cp_info **pool);

        void linkExceptionAttribute(cp_info **pool, Exceptions_attribute *attr);

        void linkCodeAttribute(cp_info **pool, Code_attribute *attr);

        bool isPcCorrect(u4 pc);

    public:
        Method(InstanceKlass *clazz, method_info *methodInfo);

        void linkMethod(cp_info **pool);

        /**
         * Used for Java calls.
         * Parse descriptor and map arguments to value types
         * short, boolean, bool and char will be wrapped to int
         * @return argument value type mapping parsed from descriptor
         */
        const std::vector<ValueType> &getArgumentValueTypes();

        /**
         * Parse descriptor and map result type to value types
         * short, boolean, bool and char will be wrapped to int
         * @return result value type parsed from descriptor
         */
        ValueType getReturnType();

        /**
         * Used for JNI calls.
         * Parse descriptor and map arguments to value types
         * short, boolean, bool and char will remains its original value type
         * @return argument value type mapping parsed from descriptor
         */
        const std::vector<ValueType> &getArgumentValueTypesNoWrap();

        /**
         * Parse descriptor and map result type to value types
         * short, boolean, bool and char will remains its original value type
         * @return result value type parsed from descriptor
         */
        ValueType getReturnTypeNoWrap();

        /**
         * Locate native method address
         * @return address of the native method
         */
        void *getNativePointer();

    public:
        int findExceptionHandler(u4 currentPc, InstanceKlass *exceptionClass);

        /*
         * Public getters and setters
         */

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
            return _accessFlag;
        }

        const CodeBlob &getCodeBlob() const {
            return _codeBlob;
        }

        bool checkAnnotation(const String &annotationName);

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
            return _codeAttr != nullptr ? _codeAttr->max_locals : 0;
        }

        int getMaxStack() const {
            return _codeAttr != nullptr ? _codeAttr->max_stack : 0;
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
