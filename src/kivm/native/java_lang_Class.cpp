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
            std::queue<String> &Class::getDelayedMirrors() {
                static std::queue<String> mirrors;
                return mirrors;
            }

            Class::ClassMirrorState &Class::getMirrorState() {
                static ClassMirrorState state = NOT_FIXED;
                return state;
            }

            std::unordered_map<String, mirrorOop> &Class::getPrimitiveTypeMirrors() {
                static std::unordered_map<String, mirrorOop> mirrors;
                return mirrors;
            }

            void Class::initialize() {
                BootstrapClassLoader::get()->loadClass(L"java/lang/Class");
                auto &m = getDelayedMirrors();
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
                getMirrorState() = ClassMirrorState::NOT_FIXED;
            }

            void Class::mirrorCoreClasses() {
                assert(getMirrorState() != ClassMirrorState::FIXED);
                assert(SystemDictionary::get()->find(L"java/lang/Class") != nullptr);

                auto &M = getDelayedMirrors();
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
                        assert(klass->getJavaMirror() == nullptr);

                        mirrorOop mirror = mirrorKlass::newMirror(klass, nullptr);
                        klass->setJavaMirror(mirror);

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
                                mirrorOop mirror = mirrorKlass::newMirror(nullptr, nullptr);
                                getPrimitiveTypeMirrors().insert(std::make_pair(name, mirror));

                                if (is_primitive_array) {
                                    // Only arrays need them.
                                    auto *array_klass = BootstrapClassLoader::get()->loadClass(name);
                                    mirror->setMirrorTarget(array_klass);
                                    array_klass->setJavaMirror(mirror);
                                } else {
                                    mirror->setMirroringPrimitiveType(primitiveTypeToValueType(primitive_type));
                                }

                                break;
                            }
                            default:
                                PANIC("Cannot make mirror for primitive type %c.", (char) primitive_type);
                        }
                    }
                }
                getMirrorState() = ClassMirrorState::FIXED;
            }
        }
    }
}
