//
// Created by kiva on 2018/3/28.
//
#include <kivm/runtime/thread.h>
#include <kivm/native/java_lang_Class.h>

namespace kivm {
    void Threads::initializeJVM() {
        // TODO: put initialization logic here.
        java::lang::Class::initialize();
        java::lang::Class::mirror_core_classes();
    }
}
