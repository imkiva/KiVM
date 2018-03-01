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

#define CNI_NAME_MIXER(PREFIX, NAME) static cni_register PREFIX##NAME
#define CNI_REGISTER(NAME, ARGS) CNI_NAME_MIXER(_cni_register_, NAME)(#NAME, NAME, ARGS);
#define CNI_NORMAL(name) CNI_REGISTER(name, false)
#define CNI_CONST(name) CNI_REGISTER(name, true)

namespace kivm_cs {
    using kivm_holder = int;

    void kivm_hello_world() {
        printf("Hello World: CovScript\n");
    }
    CNI_NORMAL(kivm_hello_world)

    void init() {
        // TODO: Initialize CovScript Extension
    }
}

namespace cs_impl {
    template<>
    cs::extension_t &get_ext<kivm_cs::kivm_holder>() {
        return kivm_ext_shared;
    }
}

cs::extension *cs_extension() {
    kivm_cs::init();
    return &kivm_ext;
}

