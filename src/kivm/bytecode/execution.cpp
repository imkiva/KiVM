//
// Created by kiva on 2018/3/27.
//
#include <kivm/bytecode/execution.h>
#include <kivm/oop/instanceOop.h>
#include <kivm/method.h>

namespace kivm {
    void Execution::initializeClass(JavaThread *javaThread, InstanceKlass *klass) {
        if (klass->getClassState() == ClassState::LINKED) {
            klass->setClassState(ClassState::BEING_INITIALIZED);
            D("Initializing class %s",
              strings::to_std_string(klass->getName()).c_str());

            // Initialize super classes first.
            Klass *super_klass = klass->getSuperClass();
            if (super_klass != nullptr) {
                Execution::initializeClass(javaThread, (InstanceKlass *) super_klass);
            }

            auto *clinit = klass->findVirtualMethod(L"<clinit>", L"()V");
            if (clinit != nullptr && clinit->getClass() == klass) {
                D("<clinit> found in %s, invoking.",
                  strings::to_std_string(klass->getName()).c_str());
                javaThread->runMethod(clinit, {});
            } else {
                D("<clinit> not found in %s, skipping.",
                  strings::to_std_string(klass->getName()).c_str());
            }
            klass->setClassState(ClassState::FULLY_INITIALIZED);
        }
    }

    void Execution::callDefaultConstructor(JavaThread *javaThread, instanceOop oop) {
        auto klass = (InstanceKlass *) oop->getClass();
        auto default_init = klass->findVirtualMethod(L"<init>", L"()V");
        assert(default_init != nullptr);
        javaThread->runMethod(default_init, {oop});
    }

    void Execution::callVoidMethod(JavaThread *javaThread, Method *method, const std::list<oop> &args) {
        javaThread->runMethod(method, args);
    }
}
