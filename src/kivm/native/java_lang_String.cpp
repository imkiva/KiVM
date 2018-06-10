//
// Created by kiva on 2018/3/29.
//

#include <kivm/native/classNames.h>
#include <kivm/native/java_lang_String.h>
#include <kivm/oop/instanceOop.h>
#include <kivm/oop/primitiveOop.h>
#include <kivm/oop/arrayOop.h>
#include <kivm/bytecode/execution.h>
#include <unordered_map>
#include <sstream>

namespace kivm {
    namespace java {
        namespace lang {
            InternStringPool *InternStringPool::getGlobal() {
                static InternStringPool global;
                return &global;
            }

            instanceOop InternStringPool::findOrNew(const kivm::String &string) {
                static Lock internLock;
                LockGuard lockGuard(internLock);

                instanceOop javaString = String::from(string);
                // Find cached string oop
                const auto &iter = _pool.find(javaString);
                if (iter != _pool.end()) {
                    return *iter;
                }

                // No cache, create new.
                _pool.insert(javaString);
                printf("intern string pool size: %zd\n", _pool.size());
                return javaString;
            }

            int StringHash::operator()(instanceOop string) const noexcept {
                // if has a hash_val cache, need no calculate.
                oop hashOop = nullptr;

                auto klass = (InstanceKlass *) string->getClass();
                FieldID *hashFieldId = klass->getInstanceFieldInfo(J_STRING, L"hash", L"I");
                if (string->getFieldValue(hashFieldId, &hashOop)) {
                    int cachedHash = ((intOop) hashOop)->getValue();
                    if (cachedHash != 0) {
                        return cachedHash;
                    }
                }

                // get string's content which is typed `TypeArrayOop` and calculate hash value.
                typeArrayOop valueOop = nullptr;
                if (string->getFieldValue(J_STRING, L"value", L"[C", (oop *) &valueOop)) {
                    int length = valueOop->getLength();
                    int hash = 0;
                    for (int i = 0; i < length; i++) {
                        auto charElement = (intOop) valueOop->getElementAt(i);
                        hash = 31 * hash + charElement->getValue();
                    }
                    string->setFieldValue(hashFieldId, new intOopDesc(hash));
                    return hash;
                }

                return 0;
            }

            int StringHash::operator()(const kivm::String &string) const noexcept {
                int hash = 0;
                for (wchar_t ch : string) {
                    hash = 31 * hash + (unsigned short) ch;
                }
                return hash;
            }

            bool StringEqualTo::operator()(instanceOop lhs, instanceOop rhs) const {
                if (lhs == rhs) {
                    return true;
                }
                if (lhs->getClass() != rhs->getClass()) {
                    return false;
                }

                auto klass = (InstanceKlass *) lhs->getClass();
                FieldID *valueFieldId = klass->getInstanceFieldInfo(J_STRING, L"value", L"[C");
                typeArrayOop lhsValue = nullptr;
                typeArrayOop rhsValue = nullptr;
                if (!lhs->getFieldValue(valueFieldId, (oop *) &lhsValue)
                    || !rhs->getFieldValue(valueFieldId, (oop *) &rhsValue)) {
                    return false;
                }

                int lhsLength = lhsValue->getLength();
                int rhsLength = rhsValue->getLength();
                if (lhsLength != rhsLength) {
                    return false;
                }

                for (int i = 0; i < lhsLength; ++i) {
                    auto lhsChar = (intOop) lhsValue->getElementAt(i);
                    auto rhsChar = (intOop) rhsValue->getElementAt(i);
                    if (lhsChar->getValue() != rhsChar->getValue()) {
                        return false;
                    }
                }
                return true;
            }

            instanceOop String::from(const kivm::String &string) {
                auto *charArrayKlass = (TypeArrayKlass *) BootstrapClassLoader::get()->loadClass(L"[C");
                auto *stringKlass = (InstanceKlass *) BootstrapClassLoader::get()->loadClass(J_STRING);

                typeArrayOop chars = charArrayKlass->newInstance((int) string.size());
                for (int i = 0; i < string.size(); ++i) {
                    chars->setElementAt(i, new intOopDesc((unsigned short) string[i]));
                }

                instanceOop javaString = stringKlass->newInstance();
                javaString->setFieldValue(J_STRING, L"value", L"[C", chars);
                return javaString;
            }

            kivm::String String::toNativeString(instanceOop stringOop) {
                typeArrayOop valueOop = nullptr;
                std::wstringstream builder;
                if (stringOop->getFieldValue(J_STRING, L"value", L"[C", (oop *) &valueOop)) {
                    int length = valueOop->getLength();
                    for (int i = 0; i < length; i++) {
                        auto charElement = (intOop) valueOop->getElementAt(i);
                        builder << (wchar_t) charElement->getValue();
                    }
                }
                return builder.str();
            }
        }
    }
}

using namespace kivm;

JAVA_NATIVE jstring Java_java_lang_String_intern(JNIEnv *env, jobject javaString) {
    auto stringOop = (instanceOop) Resolver::instance(javaString);
    const String &nativeString = java::lang::String::toNativeString(stringOop);
    return java::lang::String::intern(nativeString);
}
