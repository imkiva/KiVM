//
// Created by kiva on 2018/3/28.
//
#include <kivm/native/java_lang_Class.h>
#include <kivm/classLoader.h>

namespace kivm {
    namespace java {
        namespace lang {
            std::queue<String> &Class::delayed_mirrors() {
                static std::queue<String> mirrors;
                return mirrors;
            }

            Class::ClassMirrorState &Class::mirror_state() {
                static ClassMirrorState state = NOT_FIXED;
                return state;
            }

            void Class::initialize() {
                BootstrapClassLoader::get()->loadClass(L"java/lang/Class");
                auto &m = delayed_mirrors();
                m.push(L"I");
                m.push(L"Z");
                m.push(L"B");
                m.push(L"C");
                m.push(L"S");
                m.push(L"F");
                m.push(L"J");
                m.push(L"D");
                m.push(L"V");
                m.push(L"[I");
                m.push(L"[Z");
                m.push(L"[B");
                m.push(L"[C");
                m.push(L"[S");
                m.push(L"[F");
                m.push(L"[J");
                m.push(L"[D");
                mirror_state() = ClassMirrorState::NOT_FIXED;
            }

            void Class::mirror_core_classes() {
                assert(mirror_state() == ClassMirrorState::NOT_FIXED);
            }

            mirrorOop Class::mirror_for_primitive_type() {
                return nullptr;
            }
        }
    }
}
