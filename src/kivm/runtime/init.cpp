//
// Created by kiva on 2018/3/28.
//
#include <kivm/runtime/thread.h>
#include <kivm/bytecode/execution.h>
#include <kivm/oop/primitiveOop.h>
#include <kivm/oop/arrayKlass.h>
#include <kivm/oop/arrayOop.h>
#include <kivm/native/classNames.h>
#include <kivm/native/java_lang_Class.h>
#include <kivm/native/java_lang_Thread.h>
#include <kivm/native/java_lang_String.h>
#include <kivm/bytecode/invocationContext.h>
#include <kivm/native/java_lang_reflect_Constructor.h>
#include <kivm/native/java_lang_reflect_Method.h>

namespace kivm {
    static inline InstanceKlass *use(ClassLoader *cl, JavaMainThread *thread, const String &name) {
        auto klass = (InstanceKlass *) cl->loadClass(name);
        if (klass == nullptr) {
            PANIC("java.lang.LinkError: class not found: %s",
                strings::toStdString(name).c_str());
        }
        Execution::initializeClass(thread, klass);
        return klass;
    }

    JavaMainThread::JavaMainThread(const String &mainClassName, const std::vector<String> &arguments)
        : JavaThread(nullptr, {}),
          _mainClassName(mainClassName), _arguments(arguments) {
    }

    void JavaMainThread::start() {
        // Initialize Java Virtual Machine
        Threads::initializeJVM(this);

        // TODO: invoke main(String[])
        D("Threads::initializeJVM() succeeded. Lunching main()");

        auto mainClass = (InstanceKlass *) BootstrapClassLoader::get()->loadClass(_mainClassName);
        if (mainClass == nullptr) {
            PANIC("main class not found: %s", strings::toStdString(_mainClassName).c_str());
        }

        auto mainMethod = mainClass->getStaticMethod(L"main", L"([Ljava/lang/String;)V");
        if (mainMethod == nullptr) {
            PANIC("method main(String[]) not found in class %s",
                strings::toStdString(_mainClassName).c_str());
        }

        auto stringArrayClass = (ObjectArrayKlass *) BootstrapClassLoader::get()->loadClass(L"[Ljava/lang/String;");
        auto argumentArrayOop = stringArrayClass->newInstance((int) _arguments.size());

        for (int i = 0; i < argumentArrayOop->getLength(); ++i) {
            auto stringOop = java::lang::String::intern(_arguments[i]);
            argumentArrayOop->setElementAt(i, stringOop);
        }

        this->_method = mainMethod;
        this->_args.push_back(argumentArrayOop);
        InvocationContext::invokeWithArgs(this, _method, _args);
    }

    void JavaMainThread::onThreadLaunched() {
        // Start the first app thread to run main(String[])
        this->_nativeThread->join();

        // Then, let's wait for all app threads to finish
        for (;;) {
            int threads = Threads::getRunningJavaThreadCountLocked();
            assert(threads >= 0);

            D("scheduler: remaining app thread count: %d", threads);

            if (threads == 0) {
                break;
            }

            sched_yield();
        }
    }

    void Threads::initializeJVM(JavaMainThread *thread) {
        // TODO: put initialization logic here.
        auto cl = BootstrapClassLoader::get();

        Threads::initializeVMStructs(cl, thread);

        use(cl, thread, J_STRING);
        auto thread_class = use(cl, thread, J_THREAD);
        auto tg_class = use(cl, thread, J_THREAD_GROUP);

        // Create the init thread
        instanceOop init_thread = thread_class->newInstance();
        // eetop is a pointer to the underlying OS-level native thread instance of the JVM.
        init_thread->setFieldValue(J_THREAD, L"eetop", L"J",
            new longOopDesc(thread->getEetop()));
        init_thread->setFieldValue(J_THREAD, L"priority", L"I",
            new intOopDesc(java::lang::ThreadPriority::NORMAL_PRIORITY));

        // JavaMainThread is created with javaThreadObject == nullptr
        // Now we have created a thread for it.
        thread->setJavaThreadObject(init_thread);

        // do not addJavaThread again
        // Threads::addJavaThread(thread);

        // Create and construct the system thread group.
        instanceOop init_tg = tg_class->newInstance();
        auto tgDefaultCtor = tg_class->getThisClassMethod(L"<init>", L"()V");
        InvocationContext::invokeWithArgs(thread, tgDefaultCtor, {init_tg});

        // Create the main thread group
        instanceOop main_tg = tg_class->newInstance();
        init_thread->setFieldValue(J_THREAD, L"group", L"Ljava/lang/ThreadGroup;", main_tg);

        // Load system classes.
        auto system_class = (InstanceKlass *) cl->loadClass(L"java/lang/System");
        system_class->setClassState(ClassState::BEING_INITIALIZED);
        use(cl, thread, J_INPUT_STREAM);
        use(cl, thread, J_PRINT_STREAM);
        use(cl, thread, J_SECURITY_MANAGER);

        // Construct the main thread group
        // use getThisClassMethod() to get a private method
        auto tg_ctor = tg_class->getThisClassMethod(L"<init>",
            L"(Ljava/lang/Void;Ljava/lang/ThreadGroup;Ljava/lang/String;)V");
        {
            std::list<oop> args;
            args.push_back(main_tg);
            // we need to push `nullptr` into list
            // so do not use something like {nullptr, ...}
            args.push_back(nullptr);
            args.push_back(init_tg);
            args.push_back(java::lang::String::intern(L"main"));
            InvocationContext::invokeWithArgs(thread, tg_ctor, args);
        }


        // disable sun.security.util.Debug for the following operations
        auto sunDebug_class = cl->loadClass(L"sun/security/util/Debug");
        sunDebug_class->setClassState(ClassState::BEING_INITIALIZED);

        // Construct the init thread by attaching the main thread group to it.
        auto thread_ctor = thread_class->getThisClassMethod(L"<init>",
            L"(Ljava/lang/ThreadGroup;Ljava/lang/String;)V");
        InvocationContext::invokeWithArgs(thread, thread_ctor,
            {init_thread, main_tg, java::lang::String::intern(L"main")});

        // TODO: java.nio.charset.Charset.forName() cannot find any charsets
        hackJavaClasses(cl, thread);

        // Initialize system classes.
        auto init_system_classes = system_class->getStaticMethod(L"initializeSystemClass", L"()V");
        InvocationContext::invokeWithArgs(thread, init_system_classes, {});

        // re-enable sun.security.util.Debug
        sunDebug_class->setClassState(ClassState::FULLY_INITIALIZED);
    }

    void Threads::initializeVMStructs(BootstrapClassLoader *cl, JavaMainThread *thread) {
        java::lang::Class::initialize();
        auto java_lang_Class = use(cl, thread, J_CLASS);
        java::lang::Class::mirrorCoreAndDelayedClasses();
        java::lang::Class::mirrorDelayedArrayClasses();
        Global::java_lang_Object = use(cl, thread, J_OBJECT);
        Global::java_lang_Cloneable = use(cl, thread, J_CLONEABLE);
        Global::java_lang_Serializable = use(cl, thread, J_SERIALIZABLE);
        java::lang::reflect::Constructor::initialize();
        java::lang::reflect::Method::initialize();

        java_lang_Class->setStaticFieldValue(J_CLASS, L"useCaches", L"Z", new intOopDesc(false));
    }

    void Threads::hackJavaClasses(BootstrapClassLoader *cl, JavaMainThread *thread) {
        // hack java.nio.charset.Charset.defaultCharset
        auto charsetClass = (InstanceKlass *) BootstrapClassLoader::get()
            ->loadClass(L"java/nio/charset/Charset");
        Execution::initializeClass(thread, charsetClass);
        auto utf8CharsetClass = (InstanceKlass *) BootstrapClassLoader::get()
            ->loadClass(L"sun/nio/cs/UTF_8");
        Execution::initializeClass(thread, utf8CharsetClass);
        Global::DEFAULT_UTF8_CHARSET = utf8CharsetClass->newInstance();
        charsetClass->setStaticFieldValue(charsetClass->getName(),
            L"defaultCharset", L"Ljava/nio/charset/Charset;",
            Global::DEFAULT_UTF8_CHARSET);

        // The extremely powerful magic
        auto encoder = (InstanceKlass *) BootstrapClassLoader::get()
            ->loadClass(L"sun/nio/cs/StreamEncoder");
        auto method = encoder->getStaticMethod(L"forOutputStreamWriter",
            L"(Ljava/io/OutputStream;Ljava/lang/Object;Ljava/lang/String;)Lsun/nio/cs/StreamEncoder;");
        method->hackAsNative();

        // TODO: support System.load() and System.loadLibrary()
        auto systemClass = (InstanceKlass *) BootstrapClassLoader::get()
            ->loadClass(L"java/lang/System");
        auto loadMethod = systemClass->getStaticMethod(L"load", L"(Ljava/lang/String;)V");
        loadMethod->hackAsNative();
        auto loadLibraryMethod = systemClass->getStaticMethod(L"loadLibrary", L"(Ljava/lang/String;)V");
        loadLibraryMethod->hackAsNative();
    }
}
