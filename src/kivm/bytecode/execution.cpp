//
// Created by kiva on 2018/3/27.
//
#include <kivm/bytecode/execution.h>

namespace kivm {
    void Execution::initialize_class(InstanceKlass *klass, JavaThread *javaThread) {
        if (klass->get_state() == ClassState::LINKED) {
            klass->set_state(ClassState::BEING_INITIALIZED);
            D("Initializing class %s",
              strings::to_std_string(klass->get_name()).c_str());

            // Initialize super classes first.
            Klass *super_klass = klass->get_super_class();
            if (super_klass != nullptr) {
                Execution::initialize_class((InstanceKlass *) super_klass, javaThread);
            }

            auto *clinit = klass->find_non_virtual_method(L"<clinit>", L"()V");
            if (clinit != nullptr) {
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
}
