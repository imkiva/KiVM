#include <kivm/classpath/classLoader.h>
#include <kivm/oop/instanceKlass.h>
#include <kivm/runtime/javaThread.h>
#include <kivm/classpath/classPathManager.h>
#include <kivm/runtime/runtimeConfig.h>

int main(int argc, const char **argv) {
    using namespace kivm;

    if (argc == 1) {
        fprintf(stderr, "Usage: java [options] <class-name> [args]\n");
        return 1;
    }

    // skip argv[0]
    ++argv;

    String mainClassName = strings::replaceAll(argv[0],
        Global::DOT, Global::SLASH);
    D("java: main class name: %s\n", mainClassName.c_str());

    JavaVM *javaVM = nullptr;
    JNIEnv *env = nullptr;
    if (JNI_CreateJavaVM(&javaVM, (void **) &env, nullptr) != JNI_OK) {
        PANIC("JNI_CreateJavaVM() failed");
    }

    std::vector<String> arguments;
    // skip main class name
    ++argv;
    while (*argv) {
        arguments.push_back(*argv++);
    }

    JavaMainThread javaMainThread(mainClassName, arguments);
    javaMainThread.start();

    javaVM->DestroyJavaVM();
    return 0;
}
