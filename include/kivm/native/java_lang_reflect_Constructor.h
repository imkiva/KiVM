//
// Created by kiva on 2018/5/24.
//
#pragma once

#include <kivm/oop/instanceKlass.h>

namespace kivm {
    namespace java {
        namespace lang {
            namespace reflect {
                struct Constructor {
                    static void initialize();

                    static InstanceKlass *CLASS;
                    static FieldID *FIELD_CLAZZ;
                    static FieldID *FIELD_SLOT;
                };
            }
        }
    }
}
