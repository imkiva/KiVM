//
// Created by kiva on 2018/3/28.
//
#include <kivm/native/java_lang_Class.h>
#include <kivm/classLoader.h>
#include <kivm/oop/klass.h>
#include <kivm/oop/mirrorKlass.h>
#include <kivm/oop/mirrorOop.h>

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

            std::unordered_map<String, mirrorOop> &Class::primitive_type_mirrors() {
                static std::unordered_map<String, mirrorOop> mirrors;
                return mirrors;
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
                assert(mirror_state() != ClassMirrorState::FIXED);
                assert(SystemDictionary::get()->find(L"java/lang/Class") != nullptr);

                auto &M = delayed_mirrors();
                for (int i = 0; i < M.size(); ++i) {
                    const auto &name = M.front();
                    M.pop();

                    bool is_primitive_array = false;
                    wchar_t primitive_type = name[0];
                    if (name.size() == 2 && name[0] == 'L') {
                        is_primitive_array = true;
                        primitive_type = name[1];
                    }

                    if (name.size() > 2) {
                        Klass *klass = SystemDictionary::get()->find(name);
                        assert(klass != nullptr);
                        assert(klass->get_java_mirror() == nullptr);

                        mirrorOop mirror = mirrorKlass::new_mirror(klass, nullptr);
                        klass->set_java_mirror(mirror);

                    } else {
                        if (primitive_type == L'V' && is_primitive_array) {
                            PANIC("Cannot make mirror for void array.");
                        }

                        switch (primitive_type) {
                            case L'I':
                            case L'Z':
                            case L'B':
                            case L'C':
                            case L'S':
                            case L'F':
                            case L'J':
                            case L'D':
                            case L'V': {
                                mirrorOop mirror = mirrorKlass::new_mirror(nullptr, nullptr);
                                primitive_type_mirrors().insert(std::make_pair(name, mirror));

                                if (is_primitive_array) {
                                    // Only arrays need them.
                                    auto *array_klass = BootstrapClassLoader::get()->loadClass(name);
                                    mirror->set_mirror_target(array_klass);
                                    array_klass->set_java_mirror(mirror);
                                } else {
                                    mirror->set_mirroring_primitive_type(primitive_type_to_value_type(primitive_type));
                                }

                                break;
                            }
                            default:
                                PANIC("Cannot make mirror for primitive type %c.", (char) primitive_type);
                        }
                    }
                }
                mirror_state() = ClassMirrorState::FIXED;
            }
        }
    }
}
