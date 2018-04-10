//
// Created by kiva on 2018/3/1.
//

#pragma once
#include <covscript/cni.hpp>
#include <covscript/extension.hpp>

#define __CS_EXTENSION_NS_NAME(NAME) cs_ext_##NAME
#define __CNI_NAME_MIXER(PREFIX, NAME) static cni_register PREFIX##NAME
#define __CNI_REGISTER(NAME, ARGS) __CNI_NAME_MIXER(_cni_register_, NAME)(#NAME, NAME, ARGS);
#define __CNI_NORMAL(NAME) __CNI_REGISTER(NAME, false)
#define __CNI_CONST(NAME) __CNI_REGISTER(NAME, true)

#define __CNI_EXPAND(MARCO, R, NAME, ...) \
    extern R NAME(__VA_ARGS__); \
    MARCO(NAME); \
    R NAME(__VA_ARGS__)

#define CS_EXTENSION(NAME) \
    namespace __CS_EXTENSION_NS_NAME(NAME) { \
    static cs::extension __cs_extension_ext; \
    static cs::extension_t __cs_extension_shared = cs::make_shared_extension(__cs_extension_ext); \
    class cni_register final { \
    public: \
        template<typename _fT> \
        cni_register(const char *name, _fT &&func, bool is_const) { \
            using namespace cs; \
            __cs_extension_ext.add_var(name, cs::var::make_protect<cs::callable>(cs::cni(func), is_const)); \
        } \
    }; \
    } \
    cs::extension *cs_extension() { \
        return &__CS_EXTENSION_NS_NAME(NAME)::__cs_extension_ext; \
    } \
    namespace __CS_EXTENSION_NS_NAME(NAME) {

#define CS_EXTENSION_END() }

#define CS_DECLARE_AS_OBJECT(NAME, TYPE) \
    } \
    namespace cs_impl { \
        template<> \
        cs::extension_t &get_ext<__CS_EXTENSION_NS_NAME(NAME)::TYPE>() { \
            return __CS_EXTENSION_NS_NAME(NAME)::__cs_extension_shared; \
        } \
    } \
    namespace __CS_EXTENSION_NS_NAME(NAME) {

#define CS_OBJECT(EXT, NAME) \
    struct NAME; \
    CS_DECLARE_AS_OBJECT(EXT, NAME) \
    struct NAME

#define CNI_NORMAL(R, NAME, ...) __CNI_EXPAND(__CNI_NORMAL, R, NAME, ##__VA_ARGS__)
#define CNI_CONST(R, NAME, ...) __CNI_EXPAND(__CNI_CONST, R, NAME, ##__VA_ARGS__)

