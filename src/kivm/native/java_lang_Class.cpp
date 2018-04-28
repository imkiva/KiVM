//
// Created by kiva on 2018/3/28.
//
#include <kivm/native/java_lang_Class.h>
#include <kivm/native/java_lang_String.h>
#include <kivm/classpath/classLoader.h>
#include <kivm/oop/klass.h>
#include <kivm/oop/mirrorKlass.h>
#include <kivm/oop/mirrorOop.h>
#include <kivm/bytecode/execution.h>

namespace kivm {
    namespace java {
        namespace lang {
            std::queue<kivm::String> &Class::getDelayedMirrors() {
                static std::queue<kivm::String> mirrors;
                return mirrors;
            }

            std::unordered_map<kivm::String, mirrorOop> &Class::getPrimitiveTypeMirrors() {
                static std::unordered_map<kivm::String, mirrorOop> mirrors;
                return mirrors;
            }

            Class::ClassMirrorState &Class::getMirrorState() {
                static ClassMirrorState state = NOT_FIXED;
                return state;
            }

            mirrorOop Class::findPrimitiveTypeMirror(const kivm::String &signature) {
                auto &mirrors = getPrimitiveTypeMirrors();
                auto it = mirrors.find(signature);
                if (it != mirrors.end()) {
                    return it->second;
                }
                return nullptr;
            }

            void Class::initialize() {
                D("native: Class::initialize()");
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

            void Class::mirrorCoreAndDelayedClasses() {
                D("native: Class::mirrorCoreAndDelayedClasses()");
                assert(getMirrorState() != ClassMirrorState::FIXED);
                assert(SystemDictionary::get()->find(L"java/lang/Class") != nullptr);

                auto &M = getDelayedMirrors();
                long size = M.size();
                for (int i = 0; i < size; ++i) {
                    const auto &name = M.front();
                    M.pop();

                    bool isPrimitiveArray = false;
                    wchar_t primitiveType = name[0];
                    if (name.size() == 2 && name[0] == L'[') {
                        isPrimitiveArray = true;
                        primitiveType = name[1];
                    }

                    if (name.size() > 2) {
                        Klass *klass = SystemDictionary::get()->find(name);
                        assert(klass != nullptr);
                        assert(klass->getJavaMirror() == nullptr);

                        mirrorOop mirror = mirrorKlass::newMirror(klass, nullptr);
                        klass->setJavaMirror(mirror);
                        D("native: Class::mirrorCoreAndDelayedClasses: mirroring %s",
                          strings::toStdString(klass->getName()).c_str());

                    } else {
                        if (primitiveType == L'V' && isPrimitiveArray) {
                            PANIC("Cannot make mirror for void array.");
                        }

                        switch (primitiveType) {
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
                                D("native: Class::mirrorCoreAndDelayedClasses: mirroring primitive type %s",
                                  strings::toStdString(name).c_str());

                                if (isPrimitiveArray) {
                                    // Only arrays need them.
                                    auto *array_klass = BootstrapClassLoader::get()->loadClass(name);
                                    mirror->setMirrorTarget(array_klass);
                                    array_klass->setJavaMirror(mirror);
                                } else {
                                    mirror->setMirroringPrimitiveType(primitiveTypeToValueType(primitiveType));
                                }

                                break;
                            }
                            default:
                                PANIC("Cannot make mirror for primitive type %c",
                                      primitiveType);
                        }
                    }
                }
                getMirrorState() = ClassMirrorState::FIXED;
            }

            void Class::createMirror(Klass *klass, mirrorOop javaLoader) {
                if (getMirrorState() != ClassMirrorState::FIXED) {
                    if (klass->getClassType() == ClassType::INSTANCE_CLASS)
                        getDelayedMirrors().push(klass->getName());

                    else if (klass->getClassType() == ClassType::OBJECT_ARRAY_CLASS) {
                        PANIC("Class::createMirror(): use of deprecated mirroring policy");

                    } else {
                        PANIC("Class::createMirror(): use of illegal mirroring policy: "
                              "only instance classes are acceptable "
                              "before mirrorCoreAndDelayedClasses()");
                    }
                    return;
                }

                if (klass->getClassType() == ClassType::INSTANCE_CLASS) {
                    klass->setJavaMirror(mirrorKlass::newMirror(klass, javaLoader));

                } else if (klass->getClassType() == ClassType::TYPE_ARRAY_CLASS) {
                    mirrorOop mirror = findPrimitiveTypeMirror(klass->getName());
                    if (mirror == nullptr) {
                        mirror = mirrorKlass::newMirror(klass, nullptr);
                        getPrimitiveTypeMirrors().insert(std::make_pair(klass->getName(), mirror));
                    }
                    klass->setJavaMirror(mirror);

                } else if (klass->getClassType() == ClassType::OBJECT_ARRAY_CLASS) {
                    klass->setJavaMirror(mirrorKlass::newMirror(klass, nullptr));

                } else {
                    PANIC("Class::createMirror(): use of illegal mirroring policy: "
                          "unknown class type");
                }
            }
        }
    }
}

using namespace kivm;

JAVA_NATIVE void Java_java_lang_Class_registerNatives(JNIEnv *env, jclass java_lang_Class) {
    D("java/lang/Class.registerNatives()V");
}

JAVA_NATIVE jobject Java_java_lang_Class_getPrimitiveClass(JNIEnv *env, jclass java_lang_Class,
                                                          jstring className) {
    auto stringInstance = Resolver::resolveInstance(className);
    if (stringInstance == nullptr) {
        // TODO: throw java.lang.NullPointerException
        PANIC("java.lang.NullPointerException");
    }

    const String &signature = java::lang::String::toNativeString(stringInstance);
    if (signature == L"byte") {
        return java::lang::Class::findPrimitiveTypeMirror(L"B");

    } else if (signature == L"boolean") {
        return java::lang::Class::findPrimitiveTypeMirror(L"Z");

    } else if (signature == L"char") {
        return java::lang::Class::findPrimitiveTypeMirror(L"C");

    } else if (signature == L"short") {
        return java::lang::Class::findPrimitiveTypeMirror(L"S");

    } else if (signature == L"int") {
        return java::lang::Class::findPrimitiveTypeMirror(L"I");

    } else if (signature == L"float") {
        return java::lang::Class::findPrimitiveTypeMirror(L"F");

    } else if (signature == L"long") {
        return java::lang::Class::findPrimitiveTypeMirror(L"J");

    } else if (signature == L"double") {
        return java::lang::Class::findPrimitiveTypeMirror(L"D");

    } else if (signature == L"void") {
        return java::lang::Class::findPrimitiveTypeMirror(L"V");
    }

    PANIC("Class.getPrimitiveClass(String): unknown primitive type: %s",
          strings::toStdString(signature).c_str());
}

JAVA_NATIVE jboolean Java_java_lang_Class_desiredAssertionStatus0(JNIEnv *env, jclass java_lang_Class) {
    return JNI_FALSE;
}
