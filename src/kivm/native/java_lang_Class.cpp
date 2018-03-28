//
// Created by kiva on 2018/3/28.
//
#include <kivm/native/java_lang_Class.h>

namespace kivm {
    namespace java {
        namespace lang {
            std::queue<String>& Class::get_delayed_mirrors() {
                static std::queue<String> mirrors;
                return mirrors;
            }

            ClassMirrorState& Class::get_mirror_state() {
                static ClassMirrorState state = NOT_FIXED;
                return state;
            }

            void Class::initialize() {

            }

            void Class::fix_mirrors() {

            }

            mirrorOop Class::mirror_for_primitive_type() {
                return nullptr;
            }
        }
    }
}
