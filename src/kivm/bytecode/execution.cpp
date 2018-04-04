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
              strings::toStdString(klass->getName()).c_str());

            // Initialize super classes first.
            Klass *super_klass = klass->getSuperClass();
            if (super_klass != nullptr) {
                Execution::initializeClass(javaThread, (InstanceKlass *) super_klass);
            }

            auto *clinit = klass->getVirtualMethod(L"<clinit>", L"()V");
            if (clinit != nullptr && clinit->getClass() == klass) {
                D("<clinit> found in %s, invoking.",
                  strings::toStdString(klass->getName()).c_str());
                javaThread->runMethod(clinit, {});
            } else {
                D("<clinit> not found in %s, skipping.",
                  strings::toStdString(klass->getName()).c_str());
            }
            klass->setClassState(ClassState::FULLY_INITIALIZED);
        }
    }

    void Execution::callDefaultConstructor(JavaThread *javaThread, instanceOop oop) {
        auto klass = (InstanceKlass *) oop->getClass();
        auto default_init = klass->getVirtualMethod(L"<init>", L"()V");
        assert(default_init != nullptr);
        javaThread->runMethod(default_init, {oop});
    }

    void Execution::callVoidMethod(JavaThread *javaThread, Method *method, const std::list<oop> &args) {
        javaThread->runMethod(method, args);
    }

    void Execution::loadConstant(RuntimeConstantPool *rt, Stack &stack, int constantIndex) {
        switch (rt->getConstantTag(constantIndex)) {
            case CONSTANT_Integer: {
                stack.pushInt(rt->getInt(constantIndex));
                break;
            }
            case CONSTANT_Float: {
                stack.pushFloat(rt->getFloat(constantIndex));
                break;
            }
            case CONSTANT_Long: {
                stack.pushLong(rt->getLong(constantIndex));
                break;
            }
            case CONSTANT_Double: {
                stack.pushDouble(rt->getDouble(constantIndex));
                break;
            }
            case CONSTANT_String: {
                stack.pushReference(rt->getString(constantIndex));
                break;
            }
            case CONSTANT_Class: {
                Klass *klass = rt->getClass(constantIndex);
                auto mirror = klass->getJavaMirror();
                if (mirror == nullptr) {
                    PANIC("Pushing null classes");
                }
                stack.pushReference(mirror);
                break;
            }
            default: {
                PANIC("Unsupported constant tag");
                break;
            }
        }
    }
}
