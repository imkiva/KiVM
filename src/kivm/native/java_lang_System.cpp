//
// Created by kiva on 2018/4/21.
//

#include <compileTimeConfig.h>
#include <kivm/kivm.h>
#include <kivm/native/classNames.h>
#include <kivm/bytecode/execution.h>
#include <kivm/oop/arrayKlass.h>
#include <kivm/oop/arrayOop.h>
#include <kivm/bytecode/javaCall.h>
#include <shared/osInfo.h>
#include <sys/time.h>

using namespace kivm;

static bool isArrayRangeInvalid(jint srcPos, jint destPos, jint length,
                                arrayOopDesc *srcOop, arrayOopDesc *destOop) {
    return srcPos < 0 || destPos < 0 || length < 0
           || (((unsigned int) length + (unsigned int) srcPos) > (unsigned int) srcOop->getLength())
           || (((unsigned int) length + (unsigned int) destPos) > (unsigned int) destOop->getLength());
}

JAVA_NATIVE jobject
Java_java_lang_System_initProperties(JNIEnv *env, jclass java_lang_System, jobject propertiesObject) {
    static HashMap<String, String> PROPS{
        {L"java.vm.specification.version", L"1.8"},
        {L"path.separator", Global::PATH_SEPARATOR},
        {L"file.encoding.pkg", L"sun.io"},
        {L"os.arch", KIVM_ARCH_NAME},
        {L"os.name", OSInformation::getOSName()},
        {L"os.version", OSInformation::getOSVersion()},
        {L"sun.arch.data.model", KIVM_ARCH_DATA_MODEL},
        {L"line.separator", L"\n"},
        {L"file.separator", Global::PATH_DELIMITER},
        {L"sun.jnu.encoding", L"utf8"},
        {L"file.encoding", L"utf8"},
#if defined(KIVM_THREADED) && !defined(KIVM_DEBUG)
        {L"kivm.info", L"threaded-interpreter"}
#else
        {L"kivm.info", L"bytecode-interpreter"}
#endif
    };

    auto propOop = Resolver::instance(propertiesObject);
    auto put = propOop->getInstanceClass()->getVirtualMethod(
        L"put", L"(Ljava/lang/Object;Ljava/lang/Object;)Ljava/lang/Object;");

    auto thread = Threads::currentThread();
    assert(thread != nullptr);

    for (const auto &e : PROPS) {
        D("initProperties: set %S to %S",
            (e.first).c_str(),
            (e.second).c_str());
        std::list<oop> args{propOop,
                            java::lang::String::intern(e.first),
                            java::lang::String::intern(e.second)};
        JavaCall::withArgs(thread, put, args);
    }
    return propertiesObject;
}

JAVA_NATIVE
void Java_java_lang_System_arraycopy(JNIEnv *env, jclass java_lang_System,
                                     jobject javaSrc, jint srcPos,
                                     jobject javaDest, jint destPos,
                                     jint length) {
    auto srcOop = Resolver::array(javaSrc);
    auto destOop = Resolver::array(javaDest);

    auto thread = Threads::currentThread();
    assert(thread != nullptr);

    if (srcOop == nullptr || destOop == nullptr) {
        thread->throwException(Global::_NullPointerException, false);
        return;
    }

    if (srcOop->getClass()->getClassType() != destOop->getClass()->getClassType()) {
        thread->throwException((InstanceKlass *) BootstrapClassLoader::get()
            ->loadClass(L"java/lang/ArrayStoreException"), false);
        return;
    }

    if (srcOop->getClass()->getClassType() == ClassType::TYPE_ARRAY_CLASS) {
        auto srcOop_ = (typeArrayOop) srcOop;
        auto destOop_ = (typeArrayOop) destOop;
        auto srcClass_ = (TypeArrayKlass *) srcOop_->getClass();
        auto destClass_ = (TypeArrayKlass *) destOop_->getClass();
        if (destClass_->getComponentType() != srcClass_->getComponentType()) {
            thread->throwException((InstanceKlass *) BootstrapClassLoader::get()
                ->loadClass(L"java/lang/ArrayStoreException"), false);
            return;
        }
    } else {
        auto srcOop_ = (objectArrayOop) srcOop;
        auto destOop_ = (objectArrayOop) destOop;
        auto srcClass_ = (ObjectArrayKlass *) srcOop_->getClass();
        auto destClass_ = (ObjectArrayKlass *) destOop_->getClass();

        auto srcComponent = srcClass_->getComponentType();

        // TODO: temporary workaround, should add instanceof check
        // for toArray() in collection classes
        // for example:
        // List<Object> l = new ArrayList();
        // l.add("hello");
        // String[] s = new String[1];
        // l.toArray(s);
        if (srcComponent != Global::_Object) {
            if (srcComponent != Global::_Object
                && destClass_->getComponentType() != srcClass_->getComponentType()) {
                thread->throwException((InstanceKlass *) BootstrapClassLoader::get()
                    ->loadClass(L"java/lang/ArrayStoreException"), false);
                return;
            }
        }
    }

    // Check if the ranges are valid
    if (isArrayRangeInvalid(srcPos, destPos, length, srcOop, destOop)) {
        thread->throwException((InstanceKlass *) BootstrapClassLoader::get()
            ->loadClass(L"java/lang/ArrayIndexOutOfBoundsException"), false);
        return;
    }

    // Check zero copy
    if (length == 0) {
        return;
    }

    auto arrayClass = (ArrayKlass *) srcOop->getClass();
    arrayClass->copyArrayTo(srcOop, destOop, srcPos, destPos, length);
}

JAVA_NATIVE void Java_java_lang_System_setIn0(JNIEnv *env, jclass java_lang_System,
                                              jobject javaInputStream) {
    auto inputStreamOop = Resolver::instance(javaInputStream);
    auto system = Resolver::instanceClass(java_lang_System);
    system->setStaticFieldValue(J_SYSTEM, L"in", L"Ljava/io/InputStream;", inputStreamOop);
}

JAVA_NATIVE void Java_java_lang_System_setOut0(JNIEnv *env, jclass java_lang_System,
                                               jobject javaPrintStream) {
    auto printStreamOop = Resolver::instance(javaPrintStream);
    auto system = Resolver::instanceClass(java_lang_System);
    system->setStaticFieldValue(J_SYSTEM, L"out", L"Ljava/io/PrintStream;", printStreamOop);
}

JAVA_NATIVE void Java_java_lang_System_setErr0(JNIEnv *env, jclass java_lang_System,
                                               jobject javaPrintStream) {
    auto printStreamOop = Resolver::instance(javaPrintStream);
    auto system = Resolver::instanceClass(java_lang_System);
    system->setStaticFieldValue(J_SYSTEM, L"err", L"Ljava/io/PrintStream;", printStreamOop);
}

JAVA_NATIVE jstring Java_java_lang_System_mapLibraryName(JNIEnv *env, jclass java_lang_System,
                                                         jstring javaLibName) {
    auto stringOop = Resolver::instance(javaLibName);
    if (stringOop == nullptr) {
        auto thread = Threads::currentThread();
        assert(thread != nullptr);
        thread->throwException(Global::_NullPointerException, false);
        return nullptr;
    }

    auto libraryName = java::lang::String::toNativeString(stringOop);
#if defined(KIVM_PLATFORM_APPLE)
    auto mappedName = L"lib" + libraryName + L".dylib";
#elif defined(KIVM_PLATFORM_UNIX)
    auto mappedName = L"lib" + libraryName + L".so";
#elif defined(KIVM_PLATFORM_WINDOWS)
    auto mappedName = libraryName + L".dll";
#else
    SHOULD_NOT_REACH_HERE();
#endif
    return java::lang::String::intern(mappedName);
}

JAVA_NATIVE jint Java_java_lang_Object_hashCode(JNIEnv *env, jobject javaObject);

JAVA_NATIVE jint Java_java_lang_System_identityHashCode(JNIEnv *env, jclass java_lang_System, jobject javaObject) {
    return Java_java_lang_Object_hashCode(env, javaObject);
}

JAVA_NATIVE jlong Java_java_lang_System_currentTimeMillis(JNIEnv *env, jclass java_lang_System) {
    timeval time{};
    if (gettimeofday(&time, nullptr) == -1) {
        auto thread = Threads::currentThread();
        thread->throwException(Global::_InternalError, L"gettimeofday() failed");
        return 0;
    }
    return time.tv_sec * 1000 + time.tv_usec / 1000;
}

JAVA_NATIVE jlong Java_java_lang_System_nanoTime(JNIEnv *env, jclass java_lang_System) {
    timeval time{};
    if (gettimeofday(&time, nullptr) == -1) {
        auto thread = Threads::currentThread();
        thread->throwException(Global::_InternalError, L"gettimeofday() failed");
        return 0;
    }
    return time.tv_sec * 1000 * 1000 * 1000 + time.tv_usec * 1000;
}

// TODO: support System.load() and System.loadLibrary()
JAVA_NATIVE void Java_java_lang_System_loadLibrary(JNIEnv *, jclass, jstring) {}

JAVA_NATIVE void Java_java_lang_System_load(JNIEnv *, jclass, jstring) {}
