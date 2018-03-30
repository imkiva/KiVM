//
// Created by kiva on 2018/3/28.
//
#include <kivm/runtime/thread.h>
#include <kivm/bytecode/execution.h>
#include <kivm/oop/primitiveOop.h>
#include <kivm/native/class_names.h>
#include <kivm/native/java_lang_Class.h>
#include <kivm/native/java_lang_Thread.h>
#include <kivm/native/java_lang_String.h>

namespace kivm {
    static inline InstanceKlass *use(ClassLoader *cl, JavaMainThread *thread, const String &name) {
        auto klass = (InstanceKlass *) cl->loadClass(name);
        assert(klass != nullptr);
        Execution::initialize_class(thread, klass);
        return klass;
    }

    JavaMainThread::JavaMainThread()
        : JavaThread(nullptr, {}) {
    }

    void JavaMainThread::start() {
        // Initialize Java Virtual Machine
        Threads::initializeJVM(this);

        // TODO: find main(String[]) method and build arg list
        // OK, call main() with args
        // this->_method = main_method;
        // this->_args = main_args;
        // Run method manually, we cannot use JavaThread::run()
        // because it is designed for app threads,
        // but JavaThread::run_method() is still available.
        PANIC("JavaMainThread::start() not implemented.");
        JavaThread::run_method(_method, _args);
    }

    void JavaMainThread::thread_lunched() {
        // Start the first app thread to run main(String[])
        this->_native_thread->join();

        // Then, let's wait for all app threads to finish
        for (;;) {
            int threads = Threads::get_app_thread_count_locked();
            assert(threads >= 0);

            if (threads == 0) {
                break;
            }

            sched_yield();
        }
    }

    bool JavaMainThread::should_record_in_thread_table() {
        return false;
    }

    void Threads::initializeJVM(JavaMainThread *thread) {
        // TODO: put initialization logic here.
        auto cl = BootstrapClassLoader::get();

        java::lang::Class::initialize();
        auto class_class = use(cl, thread, J_CLASS);
        java::lang::Class::mirror_core_classes();

        auto string_class = use(cl, thread, J_STRING);
        auto thread_class = use(cl, thread, J_THREAD);
        auto tg_class = use(cl, thread, J_THREAD_GROUP);

        // Create the init thread
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

        // Create and construct the system thread group.
        instanceOop init_tg = tg_class->new_instance();
        Execution::call_default_constructor(thread, init_tg);

        // Create the main thread group
        instanceOop main_tg = tg_class->new_instance();
        init_thread->set_field_value(J_THREAD, L"group", L"Ljava/lang/ThreadGroup;", main_tg);
        class_class->set_static_field_value(J_CLASS, L"useCaches", L"Z", new intOopDesc(false));

        // Load system classes.
        auto system_class = (InstanceKlass *) cl->loadClass(L"java/lang/System");
        system_class->set_state(ClassState::BEING_INITIALIZED);
        use(cl, thread, J_INPUT_STREAM);
        use(cl, thread, J_PRINT_STREAM);
        use(cl, thread, J_SECURITY_MANAGER);

        // Construct the main thread group
        // use find_non_virtual_method() to get a private method
        auto tg_ctor = tg_class->find_non_virtual_method(L"<init>",
                                                         L"(Ljava/lang/Void;Ljava/lang/ThreadGroup;Ljava/lang/String;)V");
        Execution::call_void_method(thread, tg_ctor,
                                    {main_tg, nullptr, init_tg, java::lang::String::intern(L"main")});


        // disable sun.security.util.Debug for the following operations
        auto sunDebug_class = cl->loadClass(L"sun/security/util/Debug");
        sunDebug_class->set_state(ClassState::BEING_INITIALIZED);

        // Construct the init thread by attaching the main thread group to it.
        auto thread_ctor = thread_class->find_virtual_method(L"<init>",
                                                             L"(Ljava/lang/ThreadGroup;Ljava/lang/String;)V");
        Execution::call_void_method(thread, thread_ctor,
                                    {init_thread, main_tg, java::lang::String::intern(L"main")});


        // Initialize system classes.
        auto init_system_classes = system_class->find_static_method(L"initializeSystemClass", L"()V");
        Execution::call_void_method(thread, init_system_classes, {});

        // re-enable sun.security.util.Debug
        sunDebug_class->set_state(ClassState::FULLY_INITIALIZED);
    }
}
