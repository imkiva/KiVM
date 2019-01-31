//
// Created by kiva on 2018/2/25.
//

#pragma once

#include <cstdio>
#include <cstdint>
#include <cstddef>
#include <cassert>
#include <cstring>
#include <cstdlib>
#include <shared/types.h>
#include <shared/string.h>
#include <kivm/oop/oopfwd.h>

#ifdef KIVM_DEBUG
#define D(fmt, ...) \
    do { \
       (void) fprintf(stderr, "(debug) [%s:%d]: " fmt "\n", \
                __FILE__, \
                __LINE__, \
                ##__VA_ARGS__); \
    } while (false)
#else
#define D(fmt, ...)
#endif

#define PANIC(fmt, ...) \
    do { \
       (void) fprintf(stderr, "\n\n\n*** (panic) *** [%s:%d]: " fmt "\n\n\n", \
                __FILE__, __LINE__, ##__VA_ARGS__); \
       exit(1); \
    } while (false)

#define WARN(fmt, ...) \
    do { \
       (void) fprintf(stderr, "\n\n\n*** (warning) ***: " fmt "\n\n\n", \
                ##__VA_ARGS__); \
    } while (false)

#define SHOULD_NOT_REACH_HERE() PANIC("should not reach here")
#define WARN_SHOULD_NOT_REACH_HERE() WARN("should not reach here")

#define SHOULD_NOT_REACH_HERE_M(fmt, ...) PANIC("should not reach here: " fmt, ##__VA_ARGS__)
#define WARN_SHOULD_NOT_REACH_HERE_M(fmt, ...) WARN("should not reach here: " fmt, ##__VA_ARGS__)

#define JNI_ENTRY_NAME(nameAndSignature) jni_##nameAndSignature
#define JNI_ENTRY(returnType, nameAndSignature) returnType JNI_ENTRY_NAME(nameAndSignature)

#define JavaObject(X) instanceOop
#define JO(X) JavaObject(X)

namespace kivm {
    class InstanceKlass;

    struct Global {
        static String SLASH;
        static String DOT;
        static String UNDERLINE;
        static String PATH_SEPARATOR;
        static String PATH_DELIMITER;
        static String CLASS_EXTENSION;

        static InstanceKlass *java_lang_Object;
        static InstanceKlass *java_lang_Cloneable;
        static InstanceKlass *java_lang_Serializable;
        static InstanceKlass *java_lang_NullPointerException;
        static InstanceKlass *java_lang_ArrayIndexOutOfBoundsException;
        static InstanceKlass *java_lang_ClassNotFoundException;
        static InstanceKlass *java_lang_InternalError;
        static InstanceKlass *java_io_IOException;

        static JavaObject("java/nio/charset/Charset") DEFAULT_UTF8_CHARSET;

        static bool jvmBooted;
    };

    class JavaThread;

    class KiVM {
    private:
        static JavaVM *sJavaVMInstance;
        static JNIEnv *sJNIEnvInstance;

    private:
        static void fillInterfaceFunctions(JNINativeInterface_ *nativeInterface);

        static void uncaughtExceptionJVMInternal(instanceOop exceptionOop);

    public:
        static inline JavaVM *getJavaVMQuick() {
            return sJavaVMInstance;
        }

    public:
        static bool checkJavaVersion(int javaVersion);

        static int getJavaVM(JavaVM **pJavaVM);

        static int getEnv(JavaVM *vm, JNIEnv **pEnv, int version);

        static int createVirtualMachine(JavaVM **pJavaVM, JNIEnv **pEnv, JavaVMInitArgs *initArgs);

        static int destroyJavaVM(JavaVM *vm);

        static void uncaughtException(JavaThread *exceptionThread);
    };
}
