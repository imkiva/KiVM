//
// Created by kiva on 2018/2/27.
//

#include <sstream>
#include <utility>
#include <shared/lock.h>

#include <kivm/oop/method.h>
#include <kivm/oop/instanceKlass.h>
#include <kivm/bytecode/execution.h>
#include <kivm/native/java_lang_Class.h>
#include <kivm/jni/nativeMethod.h>

namespace kivm {
    namespace helper {
        static void argumentListParser(std::vector<ValueType> *valueTypes, bool *flag,
                                       const String &desc, bool wrap) {
            if (flag != nullptr) {
                if (*flag) {
                    return;
                }
                *flag = true;
            }

            bool isArray = false;

            for (int i = 0; i < desc.size(); ++i) {
                wchar_t ch = desc[i];
                switch (ch) {
                    case L'[' :
                        isArray = true;
                        break;

                    case L'B':    // byte
                    case L'Z':    // boolean
                    case L'S':    // short
                    case L'C':    // char
                    case L'I':    // int
                    case L'J':    // long
                    case L'F':    // float
                    case L'D':    // double
                        if (isArray) {
                            valueTypes->push_back(ValueType::ARRAY);
                            isArray = false;
                        } else {
                            valueTypes->push_back(wrap
                                                  ? primitiveTypeToValueType(ch)
                                                  : primitiveTypeToValueTypeNoWrap(ch));
                        }
                        break;

                    case L'L':
                        while (desc[i] != ';') {
                            ++i;
                        }
                        if (isArray) {
                            valueTypes->push_back(ValueType::ARRAY);
                            isArray = false;
                        } else {
                            valueTypes->push_back(ValueType::OBJECT);
                        }
                        break;

                    case L'(':
                        break;

                    case L')':
                        return;

                    default:
                        PANIC("Unrecognized char %c in descriptor", ch);
                }
            }
        }

        static void argumentListParser(std::vector<mirrorOop> *argumentTypes, bool *flag,
                                       const String &desc) {
            if (*flag) {
                return;
            }
            *flag = true;

            int startIndex = 0;
            bool inClassName = false;

            for (int i = 0; i < desc.size(); ++i) {
                wchar_t ch = desc[i];
                if (ch == L')') {
                    break;
                }

                switch (ch) {
                    case L'(':
                    case L'[' :
                        startIndex = i;
                        break;

                    case L'L':
                        inClassName = true;
                        startIndex = i;
                        break;

                    case L'B':    // byte
                    case L'Z':    // boolean
                    case L'S':    // short
                    case L'C':    // char
                    case L'I':    // int
                    case L'J':    // long
                    case L'F':    // float
                    case L'D':    // double
                    {
                        const auto &part = desc.substr(startIndex, i - startIndex);
                        D("oop: parsing arg list: primitive type: %s",
                            strings::toStdString(part).c_str());
                        startIndex = i;
                        break;
                    }

                    case ';':
                        if (inClassName) {
                            inClassName = false;
                            const auto &part = desc.substr(startIndex, i - startIndex);
                            D("oop: parsing arg list: reference type: %s",
                                strings::toStdString(part).c_str());
                        }
                        break;

                    default:
                        PANIC("Unrecognized char %c in descriptor", ch);
                }
            }
        }

        static void returnTypeParser(ValueType *returnType, bool *flag,
                                     const String &desc, bool wrap) {
            if (*flag) {
                return;
            }
            *flag = true;

            const String &returnTypeDesc = desc.substr(desc.find_first_of(L')') + 1);
            wchar_t ch = returnTypeDesc[0];
            switch (ch) {
                case L'B':    // byte
                case L'Z':    // boolean
                case L'S':    // short
                case L'C':    // char
                case L'I':    // int
                case L'J':    // long
                case L'F':    // float
                case L'D':    // double
                case L'V':    // void
                    *returnType = wrap
                                  ? primitiveTypeToValueType(ch)
                                  : primitiveTypeToValueTypeNoWrap(ch);
                    break;

                case L'L':
                    *returnType = ValueType::OBJECT;
                    break;

                case L'[':
                    *returnType = ValueType::ARRAY;
                    break;

                default:
                    PANIC("Unrecognized char %c in descriptor", ch);
            }
        }

        static void returnTypeParser(mirrorOop *returnType,
                                     const String &desc) {
            if (*returnType != nullptr) {
                return;
            }

            const String &returnTypeDesc = desc.substr(desc.find_first_of(L')') + 1);
            wchar_t ch = returnTypeDesc[0];
            switch (ch) {
                case L'B':    // byte
                case L'Z':    // boolean
                case L'S':    // short
                case L'C':    // char
                case L'I':    // int
                case L'J':    // long
                case L'F':    // float
                case L'D':    // double
                case L'V':    // void
                {
                    *returnType = java::lang::Class::findPrimitiveTypeMirror(returnTypeDesc);
                    break;
                }

                case L'L':
                case L'[': {
                    auto returnClass = BootstrapClassLoader::get()->loadClass(returnTypeDesc);
                    if (returnClass == nullptr) {
                        PANIC("cannot parse return type of method");
                    }
                    *returnType = returnClass->getJavaMirror();
                    break;
                }

                default:
                    PANIC("Unrecognized char %c in descriptor", ch);
            }
        }
    }

    static Lock &get_method_pool_lock() {
        static Lock _method_pool_lock;
        return _method_pool_lock;
    }

    void MethodPool::add(Method *method) {
        LockGuard guard(get_method_pool_lock());
        getEntriesInternal().push_back(method);
    }

    std::list<Method *> &MethodPool::getEntriesInternal() {
        static std::list<Method *> _entries;
        return _entries;
    }

    const std::list<Method *> &MethodPool::getEntries() {
        return getEntriesInternal();
    }

    bool Method::isSame(const Method *lhs, const Method *rhs) {
        return lhs != nullptr && rhs != nullptr
               && lhs->getName() == rhs->getName()
               && lhs->getDescriptor() == rhs->getDescriptor();
    }

    String Method::makeIdentity(const Method *m) {
        std::wstringstream ss;
        ss << m->getName() << L" " << m->getDescriptor();
        return ss.str();
    }

    Method::Method(InstanceKlass *clazz, method_info *methodInfo) {
        this->_linked = false;
        this->_klass = clazz;
        this->_methodInfo = methodInfo;
        this->_codeAttr = nullptr;
        this->_exceptionAttr = nullptr;
        this->_argumentValueTypesResolved = false;
        this->_argumentValueTypesNoWrapResolved = false;
        this->_returnTypeResolved = false;
        this->_returnTypeNoWrapResolved = false;
        this->_nativePointer = nullptr;
        this->_runtimeVisibleAnnos = nullptr;
        this->_returnClassType = nullptr;
    }

    bool Method::isPcCorrect(u4 pc) {
        return _codeAttr != nullptr
               && _codeAttr->code_length > 0
               && pc < _codeAttr->code_length;
    }

    void Method::linkMethod(cp_info **pool) {
        if (_linked) {
            return;
        }

        this->_accessFlag = _methodInfo->access_flags;
        auto *name_info = requireConstant<CONSTANT_Utf8_info>(pool, _methodInfo->name_index);
        auto *desc_info = requireConstant<CONSTANT_Utf8_info>(pool, _methodInfo->descriptor_index);
        this->_name = name_info->getConstant();
        this->_descriptor = desc_info->getConstant();
        linkAttributes(pool);

        if (!isAbstract() && !isNative()) {
            assert(_codeAttr != nullptr);
        }
        _linked = true;
    }

    void Method::linkAttributes(cp_info **pool) {
        for (int i = 0; i < _methodInfo->attributes_count; ++i) {
            attribute_info *attr = _methodInfo->attributes[i];

            switch (AttributeParser::toAttributeTag(attr->attribute_name_index, pool)) {
                case ATTRIBUTE_Code: {
                    linkCodeAttribute(pool, (Code_attribute *) attr);
                    break;
                }
                case ATTRIBUTE_Exceptions: {
                    linkExceptionAttribute(pool, (Exceptions_attribute *) attr);
                    break;
                }
                case ATTRIBUTE_Signature: {
                    auto *sig_attr = (Signature_attribute *) attr;
                    auto *utf8 = requireConstant<CONSTANT_Utf8_info>(pool, sig_attr->signature_index);
                    _signature = utf8->getConstant();
                    break;
                }
                case ATTRIBUTE_RuntimeVisibleAnnotations: {
                    auto r = ((RuntimeVisibleAnnotations_attribute *) attr)->parameter_annotations;
                    this->_runtimeVisibleAnnos = new ParameterAnnotation(pool, &r);
                    break;
                }
                case ATTRIBUTE_RuntimeVisibleParameterAnnotations: {
                    auto r = ((RuntimeVisibleParameterAnnotations_attribute *) attr);
                    for (int j = 0; j < r->num_parameters; ++j) {
                        auto p = r->parameter_annotations[j];
                        this->_runtimeVisibleParameterAnnos.push_back(new ParameterAnnotation(pool, &p));
                    }
                    break;
                }
                case ATTRIBUTE_RuntimeVisibleTypeAnnotations: {
                    auto r = ((RuntimeVisibleTypeAnnotations_attribute *) attr);
                    for (int j = 0; j < r->num_annotations; ++j) {
                        auto p = r->annotations[j];
                        this->_runtimeVisibleTypeAnnos.push_back(new TypeAnnotation(pool, &p));
                    }
                    break;
                }
                case ATTRIBUTE_AnnotationDefault: {
                    break;
                }
                default: {
                    break;
                }
            }
        }
    }

    void Method::linkExceptionAttribute(cp_info **pool, Exceptions_attribute *attr) {
        this->_exceptionAttr = attr;
    }

    void Method::linkCodeAttribute(cp_info **pool, Code_attribute *attr) {
        _codeAttr = attr;
        // check exception handlers
        for (int i = 0; i < attr->exception_table_length; ++i) {
            if (isPcCorrect(attr->exception_table[i].start_pc)
                && isPcCorrect(attr->exception_table[i].end_pc)
                && isPcCorrect(attr->exception_table[i].handler_pc)) {
                continue;
            }
            // TODO: throw VerifyError
            assert(false);
        }

        // link attributes
        for (int i = 0; i < attr->attributes_count; ++i) {
            attribute_info *sub_attr = attr->attributes[i];
            switch (AttributeParser::toAttributeTag(sub_attr->attribute_name_index, pool)) {
                case ATTRIBUTE_LineNumberTable: {
                    auto *line_attr = (LineNumberTable_attribute *) sub_attr;
                    for (int j = 0; j < line_attr->line_number_table_length; ++j) {
                        _lineNumberTable[line_attr->line_number_table[j].start_pc]
                            = line_attr->line_number_table[j].line_number;
                    }
                    break;
                }
                case ATTRIBUTE_RuntimeVisibleTypeAnnotations:
                case ATTRIBUTE_StackMapTable:
                case ATTRIBUTE_LocalVariableTable:
                case ATTRIBUTE_LocalVariableTypeTable:
                case ATTRIBUTE_RuntimeInvisibleTypeAnnotations:
                default:
                    // TODO
                    break;
            }
        }

        _codeBlob.init(_codeAttr->code, _codeAttr->code_length);
    }

    JavaNativeMethod *Method::getNativeMethod() {
        if (this->isNative()) {
            if (this->_nativePointer == nullptr) {
                this->_nativePointer = JavaNativeMethod::resolve(this);
            }
            return this->_nativePointer;
        }
        PANIC("non-native methods have no native pointers");
    }

    int Method::findExceptionHandler(u4 currentPc, InstanceKlass *exceptionClass) {
        auto codeAttr = this->_codeAttr;
        auto pool = this->getClass()->getRuntimeConstantPool();

        for (int i = 0; i < codeAttr->exception_table_length; ++i) {
            auto ex = codeAttr->exception_table[i];
            if (currentPc < ex.start_pc || currentPc > ex.end_pc) {
                continue;
            }

            if (ex.catch_type == 0) {
                // the finally block
                return ex.handler_pc;
            }

            auto checkClass = pool->getClass(ex.catch_type);
            if (checkClass == exceptionClass
                || Execution::instanceOf(exceptionClass, checkClass)) {
                // Yes! we got exception handler
                return ex.handler_pc;
            }
        }

        return -1;
    }

    bool Method::checkAnnotation(const String &annotationName) {
        if (_runtimeVisibleAnnos != nullptr &&
            _runtimeVisibleAnnos->checkTypeName(annotationName)) {
            return true;
        }
        for (auto &_runtimeVisibleParameterAnno : _runtimeVisibleParameterAnnos) {
            if (_runtimeVisibleParameterAnno->checkTypeName(annotationName)) {
                return true;
            }
        }
        for (auto &_runtimeVisibleTypeAnno : _runtimeVisibleTypeAnnos) {
            if (_runtimeVisibleTypeAnno->checkTypeName(annotationName)) {
                return true;
            }
        }

        return false;
    }

    const std::vector<ValueType> &Method::getArgumentValueTypes() {
        helper::argumentListParser(&_argumentValueTypes,
            &_argumentValueTypesResolved,
            getDescriptor(), true);
        return _argumentValueTypes;
    }

    ValueType Method::getReturnType() {
        helper::returnTypeParser(&_returnType,
            &_returnTypeResolved,
            getDescriptor(), true);
        return _returnType;
    }

    const std::vector<ValueType> &Method::getArgumentValueTypesNoWrap() {
        helper::argumentListParser(&_argumentValueTypesNoWrap,
            &_argumentValueTypesNoWrapResolved,
            getDescriptor(), false);
        return _argumentValueTypesNoWrap;
    }

    ValueType Method::getReturnTypeNoWrap() {
        helper::returnTypeParser(&_returnTypeNoWrap,
            &_returnTypeNoWrapResolved,
            getDescriptor(), false);
        return _returnTypeNoWrap;
    }

    mirrorOop Method::getReturnClassType() {
        helper::returnTypeParser(&_returnClassType, getDescriptor());
        return _returnClassType;
    }

    const std::vector<mirrorOop> &Method::getArgumentClassTypes() {
        helper::argumentListParser(&_argumentClassTypes, &_argumentClassTypesResolved,
            getDescriptor());
        return _argumentClassTypes;
    }

    int Method::getLineNumber(u4 pc) {
        u2 shortenPc = (u2) pc;
        if (this->isPcCorrect(shortenPc)) {
            auto iter = this->_lineNumberTable.find(shortenPc);
            return iter == _lineNumberTable.end() ? -1 : iter->second;
        }
        return 0;
    }

    //===============================================================================

    std::vector<mirrorOop> parseArguments(const String &descriptor) {
        std::vector<mirrorOop> args;
        helper::argumentListParser(&args, nullptr, descriptor);
        return std::move(args);
    }

    mirrorOop parseReturnType(const String &descriptor) {
        mirrorOop ret;
        helper::returnTypeParser(&ret, descriptor);
        return ret;
    }

    std::vector<ValueType> parseArgumentValueTypes(const String &descriptor) {
        std::vector<ValueType> args;
        helper::argumentListParser(&args, nullptr, descriptor, true);
        return std::move(args);
    }

    ValueType parseReturnValueType(const String &descriptor) {
        ValueType ret;
        helper::returnTypeParser(&ret, nullptr, descriptor, true);
        return ret;
    }
}
