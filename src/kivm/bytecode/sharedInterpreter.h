//
// Created by kiva on 2018/6/3.
//
#pragma once

#undef D
#define D(...)

#define GOTO_BY_OFFSET(branch) \
                    pc += branch

#define GOTO_BY_OFFSET_WITH_OCCUPIED(branch, occupied) \
                    GOTO_BY_OFFSET(branch); \
                    GOTO_BY_OFFSET(-((occupied) - 1))

#define GOTO_BY_OFFSET_HARDCODEDED(occupied) \
                    short branch = codeBlob[pc] << 8 | codeBlob[pc + 1]; \
                    GOTO_BY_OFFSET_WITH_OCCUPIED(branch, occupied)

#define GOTO_ABSOLUTE(newPc) \
                    pc = newPc

#define GOTO_ABSOLUTE_WITH_OCCUPIED(newPc, occupied) \
                    GOTO_ABSOLUTE(newPc); \
                    GOTO_BY_OFFSET(-((occupied) - 1))

#define __IF_GOTO_FACTORY(func, target, occupied, op) \
                    if (stack.func() op target) { \
                        GOTO_BY_OFFSET_HARDCODEDED(occupied); \
                    } else { \
                        pc += (occupied); \
                    }

#define __IF_CMP_GOTO_FACTORY(func, occupied, op) \
                    auto v2 = stack.func(); \
                    auto v1 = stack.func(); \
                    if (v1 op v2) { \
                        GOTO_BY_OFFSET_HARDCODEDED(occupied); \
                    } else { \
                        pc += (occupied); \
                    }

#define IF_GOTO(occupied, op) __IF_GOTO_FACTORY(popInt, 0, occupied, op)
#define IF_NULLCMP_GOTO(occupied, op) __IF_GOTO_FACTORY(popReference, nullptr, occupied, op)

#define IF_ICMP_GOTO(occupied, op) __IF_CMP_GOTO_FACTORY(popInt, occupied, op)
#define IF_ACMP_GOTO(occupied, op) __IF_CMP_GOTO_FACTORY(popReference, occupied, op)

#define LOAD_ARRAY_ELEMENT(oopType, varName, resolveFunc, pushFunc, exp) \
    int index = stack.popInt(); \
    jobject ref = stack.popReference(); \
    auto array = Resolver::resolveFunc(ref); \
    if (array == nullptr) { \
        thread->throwException(Global::java_lang_NullPointerException); \
        stack.clear(); \
        stack.pushReference(thread->_exceptionOop); \
        goto exceptionHandler; \
    } \
    if (index < 0 || index >= array->getLength()) { \
        thread->throwException(Global::java_lang_ArrayIndexOutOfBoundsException, \
            L"length is " \
            + std::to_wstring(array->getLength()) \
            + L", but index is " \
            + std::to_wstring(index)); \
        stack.pushReference(thread->_exceptionOop); \
        goto exceptionHandler; \
    } \
    auto varName = (oopType) array->getElementAt(index);\
    stack.pushFunc(exp)

#define STORE_ARRAY_ELEMENT(varName, resolveFunc, popFunc, exp) \
    auto varName = stack.popFunc(); \
    int index = stack.popInt(); \
    auto ref = stack.popReference(); \
    auto array = Resolver::resolveFunc(ref); \
    if (array == nullptr) { \
        thread->throwException(Global::java_lang_NullPointerException); \
        stack.clear(); \
        stack.pushReference(thread->_exceptionOop); \
        goto exceptionHandler; \
    } \
    if (index < 0 || index >= array->getLength()) { \
        thread->throwException(Global::java_lang_ArrayIndexOutOfBoundsException, \
            L"length is " \
            + std::to_wstring(array->getLength()) \
            + L", but index is " \
            + std::to_wstring(index)); \
        stack.pushReference(thread->_exceptionOop); \
        goto exceptionHandler; \
    } \
    array->setElementAt(index, exp);

#define HANDLE_EXCEPTION() \
     stack.clear(); \
     stack.pushReference(thread->_exceptionOop); \
     goto exceptionHandler

#define CHECK_EXCEPTION() \
    if (thread->isExceptionOccurred()) { \
        HANDLE_EXCEPTION(); \
    }

