//
// Created by kiva on 2018/3/1.
//

#include <covscript/cni.hpp>
#include <covscript/extension.hpp>

#define __CNI_NAME_MIXER(PREFIX, NAME) static cni_register PREFIX##NAME
#define __CNI_REGISTER(NAME, ARGS) __CNI_NAME_MIXER(_cni_register_, NAME)(#NAME, NAME, ARGS);
#define __CNI_NORMAL(name) __CNI_REGISTER(name, false)
#define __CNI_CONST(name) __CNI_REGISTER(name, true)

#define __CNI_EXPAND(MARCO, R, NAME, ...) \
    extern R NAME(__VA_ARGS__); \
    MARCO(NAME); \
    R NAME(__VA_ARGS__)

#define __CS_EXTENSION_NAME(NAME) __cs_extension_ext_for_##NAME
#define __CS_SHARED_EXTENSION_NAME(NAME) __cs_extension_shared_ext_for_##NAME

#define CS_EXTENSION(NAME) \
    static cs::extension __CS_EXTENSION_NAME(NAME); \
    static cs::extension_t __CS_SHARED_EXTENSION_NAME(NAME) = cs::make_shared_extension(__CS_EXTENSION_NAME(NAME)); \
    class cni_register final { \
    public: \
    template<typename _fT> \
    cni_register(const char *name, _fT &&func, bool is_const) { \
        using namespace cs; \
        __CS_EXTENSION_NAME(NAME).add_var(name, var::make_protect<callable>(cni(func), is_const)); \
    } \
    }; \
    cs::extension *cs_extension() { \
        return &__CS_EXTENSION_NAME(NAME); \
    }

#define CS_ENABLE_THIS(NAME, TYPE) \
    namespace cs_impl { \
        template<> \
        cs::extension_t &get_ext<TYPE>() { \
            return __CS_SHARED_EXTENSION_NAME(NAME); \
        } \
    }

#define CNI_NORMAL(R, NAME, ...) __CNI_EXPAND(__CNI_NORMAL, R, NAME, ##__VA_ARGS__)
#define CNI_CONST(R, NAME, ...) __CNI_EXPAND(__CNI_CONST, R, NAME, ##__VA_ARGS__)


CS_EXTENSION(kivm)

namespace kivm_cs {
    struct kivm_holder {
    };

    CNI_NORMAL(void, hello_world) {
        printf("Hello World: CovScript\n");
    }

    CNI_NORMAL(void, greet, cs::number x) {
        printf("Hello World: %d\n", static_cast<int>(x));
    }

    CNI_NORMAL(void, greet2, cs::number x, cs::number y) {
        printf("Hello World: x = %d, y = %d\n", static_cast<int>(x), static_cast<int>(y));
    }
}

CS_ENABLE_THIS(kivm, kivm_cs::kivm_holder)

