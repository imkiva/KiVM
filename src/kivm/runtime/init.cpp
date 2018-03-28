//
// Created by kiva on 2018/3/28.
//
#include <kivm/runtime/thread.h>
#include <kivm/bytecode/execution.h>
#include <kivm/oop/primitiveOop.h>
#include <kivm/native/class_names.h>
#include <kivm/native/java_lang_Class.h>
#include <kivm/native/java_lang_Thread.h>

namespace kivm {
    void Threads::initializeJVM(JavaThread *thread) {
        // TODO: put initialization logic here.
        java::lang::Class::initialize();
        java::lang::Class::mirror_core_classes();

        auto *string_class = (InstanceKlass *) BootstrapClassLoader::get()->loadClass(J_STRING);
        assert(string_class != nullptr);
        Execution::initialize_class(string_class, thread);

        auto thread_class = (InstanceKlass *) BootstrapClassLoader::get()->loadClass(J_THREAD);
        assert(thread_class != nullptr);
        Execution::initialize_class(thread_class, thread);

        instanceOop init_thread = thread_class->new_instance();
        init_thread->set_field_value(J_THREAD, L"eetop", L"J",
                                     new longOopDesc(thread->get_thread_id()));
        init_thread->set_field_value(J_THREAD, L"priority", L"I",
                                     new intOopDesc(java::lang::ThreadPriority::NORMAL_PRIORITY));
    }
}
