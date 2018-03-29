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
    static inline InstanceKlass *use(ClassLoader *cl, JavaMainThread *thread, const String &name) {
        auto klass = (InstanceKlass *) cl->loadClass(name);
        assert(klass != nullptr);
        Execution::initialize_class(thread, klass);
        return klass;
    }

    void Threads::initializeJVM(JavaMainThread *thread) {
        // TODO: put initialization logic here.
        java::lang::Class::initialize();
        java::lang::Class::mirror_core_classes();

        auto cl = BootstrapClassLoader::get();

        auto string_class = use(cl, thread, J_STRING);
        auto thread_class = use(cl, thread, J_THREAD);
        auto tg_class = use(cl, thread, J_THREAD_GROUP);

        instanceOop init_thread = thread_class->new_instance();
        // eetop is a pointer to the underlying OS-level native thread instance of the JVM.
        init_thread->set_field_value(J_THREAD, L"eetop", L"J",
                                     new longOopDesc(thread->get_eetop()));
        init_thread->set_field_value(J_THREAD, L"priority", L"I",
                                     new intOopDesc(java::lang::ThreadPriority::NORMAL_PRIORITY));

        // JavaMainThread is created with java_thread_object == nullptr
        // Now we have created a thread for it.
        thread->set_java_thread_object(init_thread);
        Threads::add(thread);

        // Create the system thread group
        instanceOop init_tg = tg_class->new_instance();
        Execution::call_default_constructor(thread, init_tg);

        // Create the main thread group
        instanceOop main_tg = tg_class->new_instance();

    }
}
