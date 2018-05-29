#include <kivm/classpath/classLoader.h>
#include <kivm/oop/instanceKlass.h>
#include <kivm/runtime/thread.h>
#include <kivm/classpath/classPathManager.h>

int main(int argc, const char **argv) {
    using namespace kivm;

    if (argc == 1) {
        fprintf(stderr, "Usage: java <class-name>\n");
        return 1;
    }

    JavaVM *javaVM = nullptr;
    JNIEnv *env = nullptr;
    if (JNI_CreateJavaVM(&javaVM, (void **) &env, nullptr) != JNI_OK) {
        PANIC("JNI_CreateJavaVM() failed");
    }

    JavaMainThread javaMainThread;
    javaMainThread.create(nullptr);

    javaVM->DestroyJavaVM();
    return 0;
}
