#include <kivm/classpath/classLoader.h>
#include <kivm/oop/instanceKlass.h>
#include <kivm/runtime/thread.h>
#include <kivm/classpath/classPathManager.h>

int main() {
    using namespace kivm;

    JavaVM *javaVM;
    JNIEnv *env;
    if (JNI_CreateJavaVM(&javaVM, (void **) &env, nullptr) != JNI_OK) {
        PANIC("JNI_CreateJavaVM() failed");
    }

    JavaMainThread javaMainThread;
    javaMainThread.create(nullptr);

    javaVM->DestroyJavaVM();
    return 0;
}
