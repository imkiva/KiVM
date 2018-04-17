#include <kivm/classLoader.h>
#include <kivm/oop/instanceKlass.h>
#include <kivm/runtime/thread.h>

int main() {
    using namespace kivm;
    JavaMainThread javaMainThread;
    javaMainThread.create(nullptr);
    return 0;
}
