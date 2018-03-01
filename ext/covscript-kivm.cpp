//
// Created by kiva on 2018/3/1.
//

#include <covscript/cni.hpp>
#include <covscript/extension.hpp>

static cs::extension kivm_ext;
static cs::extension_t kivm_ext_shared = cs::make_shared_extension(kivm_ext);

class cni_register final {
public:
    template<typename _fT>
    cni_register(const char *name, _fT &&func, bool is_const) {
        using namespace cs;
        kivm_ext.add_var(name, var::make_protect<callable>(cni(func), is_const));
    }
};

#define __CNI_NAME_MIXER(PREFIX, NAME) static cni_register PREFIX##NAME
#define __CNI_REGISTER(NAME, ARGS) __CNI_NAME_MIXER(_cni_register_, NAME)(#NAME, NAME, ARGS);
#define __CNI_NORMAL(name) __CNI_REGISTER(name, false)
#define __CNI_CONST(name) __CNI_REGISTER(name, true)

#define __CNI_EXPAND(MARCO, R, NAME, ...) \
    extern R NAME(__VA_ARGS__); \
    MARCO(NAME); \
    R NAME(__VA_ARGS__)

#define CNI_NORMAL(R, NAME, ...) __CNI_EXPAND(__CNI_NORMAL, R, NAME, ##__VA_ARGS__)
#define CNI_CONST(R, NAME, ...) __CNI_EXPAND(__CNI_CONST, R, NAME, ##__VA_ARGS__)

namespace kivm_cs {
    using kivm_holder = int;

    CNI_NORMAL(void, hello_world) {
        printf("Hello World: CovScript\n");
    }

    CNI_NORMAL(void, greeting, cs::number x) {
        printf("Hello World: %d\n", static_cast<int>(x));
    }
}

namespace cs_impl {
    template<>
    cs::extension_t &get_ext<kivm_cs::kivm_holder>() {
        return kivm_ext_shared;
    }
}

cs::extension *cs_extension() {
    return &kivm_ext;
}

