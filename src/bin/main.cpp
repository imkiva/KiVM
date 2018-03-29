#include <kivm/classLoader.h>
#include <kivm/oop/instanceKlass.h>
#include <kivm/runtime/thread.h>

int main() {
    using namespace kivm;
    auto *integer = (InstanceKlass *) BootstrapClassLoader::get()
            ->loadClass(L"java/lang/Integer");

    auto *a = (InstanceKlass *) BootstrapClassLoader::get()
            ->loadClass(L"java/lang/String");

    auto *b = (InstanceKlass *) BootstrapClassLoader::get()
            ->loadClass(L"[Ljava/lang/String;");

    auto *c = (InstanceKlass *) BootstrapClassLoader::get()
            ->loadClass(L"[[[[[Ljava/lang/String;");

    auto *d = (InstanceKlass *) BootstrapClassLoader::get()
            ->loadClass(L"[J");

    auto *e = (InstanceKlass *) BootstrapClassLoader::get()
            ->loadClass(L"[[[[[J");

    auto *x = (InstanceKlass *) BootstrapClassLoader::get()
            ->loadClass(L"java/lang/InterruptedException");

    return 0;
}
