//
// Created by kiva on 2018/5/24.
//

#include <kivm/native/classNames.h>
#include <kivm/native/java_lang_reflect_Method.h>

namespace kivm {
    namespace java {
        namespace lang {
            namespace reflect {
                InstanceKlass *Method::CLASS = nullptr;
                FieldID *Method::FIELD_CLAZZ = nullptr;
                FieldID *Method::FIELD_SLOT = nullptr;

                void Method::initialize() {
                    CLASS = (InstanceKlass *) BootstrapClassLoader::get()
                        ->loadClass("java/lang/reflect/Method");
                    FIELD_CLAZZ = CLASS->getInstanceFieldInfo(J_METHOD,
                        "clazz", "Ljava/lang/Class;");
                    FIELD_SLOT = CLASS->getInstanceFieldInfo(J_METHOD,
                        "slot", "I");
                }
            }
        }
    }
}
