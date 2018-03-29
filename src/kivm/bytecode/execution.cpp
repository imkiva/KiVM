//
// Created by kiva on 2018/3/27.
//
#include <kivm/bytecode/execution.h>
#include <kivm/oop/instanceOop.h>
#include <kivm/method.h>

namespace kivm {
    void Execution::initialize_class(JavaThread *javaThread, InstanceKlass *klass) {
        if (klass->get_state() == ClassState::LINKED) {
            klass->set_state(ClassState::BEING_INITIALIZED);
            D("Initializing class %s",
              strings::to_std_string(klass->get_name()).c_str());

            // Initialize super classes first.
            Klass *super_klass = klass->get_super_class();
            if (super_klass != nullptr) {
                Execution::initialize_class(javaThread, (InstanceKlass *) super_klass);
            }

            auto *clinit = klass->find_virtual_method(L"<clinit>", L"()V");
            if (clinit != nullptr && clinit->get_class() == klass) {
                D("<clinit> found in %s, invoking.",
                  strings::to_std_string(klass->get_name()).c_str());
                javaThread->run_method(clinit, {});
            } else {
                D("<clinit> not found in %s, skipping.",
                  strings::to_std_string(klass->get_name()).c_str());
            }
            klass->set_state(ClassState::FULLY_INITIALIZED);
        }
    }

    void Execution::call_default_constructor(JavaThread *javaThread, instanceOop oop) {
        auto klass = (InstanceKlass *) oop->get_klass();
        auto default_init = klass->find_virtual_method(L"<init>", L"()V");
        assert(default_init != nullptr);
        javaThread->run_method(default_init, {oop});
    }

    void Execution::call_void_method(JavaThread *javaThread, Method *method, const std::list<oop> &args) {

    }
}
