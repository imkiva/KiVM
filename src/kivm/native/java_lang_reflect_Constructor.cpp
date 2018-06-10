//
// Created by kiva on 2018/5/24.
//

#include <kivm/native/classNames.h>
#include <kivm/native/java_lang_reflect_Constructor.h>

namespace kivm {
    namespace java {
        namespace lang {
            namespace reflect {
                InstanceKlass *Constructor::CLASS = nullptr;
                FieldID *Constructor::FIELD_CLAZZ = nullptr;
                FieldID *Constructor::FIELD_SLOT = nullptr;

                void Constructor::initialize() {
                    CLASS = (InstanceKlass *) BootstrapClassLoader::get()
                        ->loadClass("java/lang/reflect/Constructor");
                    FIELD_CLAZZ = CLASS->getInstanceFieldInfo(J_CTOR,
                        "clazz", "Ljava/lang/Class;");
                    FIELD_SLOT = CLASS->getInstanceFieldInfo(J_CTOR,
                        "slot", "I");
                }
            }
        }
    }
}
