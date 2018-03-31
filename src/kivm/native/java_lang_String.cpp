//
// Created by kiva on 2018/3/29.
//

#include <kivm/native/class_names.h>
#include <kivm/native/java_lang_String.h>
#include <kivm/oop/instanceOop.h>
#include <kivm/oop/primitiveOop.h>
#include <kivm/oop/arrayOop.h>
#include <unordered_map>

namespace kivm {
    namespace java {
        namespace lang {
            int String::Hash::operator()(instanceOop string) const noexcept {
                // if has a hash_val cache, need no calculate.
                oop int_oop_hash = nullptr;

                auto klass = (InstanceKlass *) string->getClass();
                FieldID hash_field = klass->getInstanceFieldInfo(J_STRING, L"hash", L"I");
                if (string->getFieldValue(hash_field, &int_oop_hash)) {
                    int cached_hash = ((intOop) int_oop_hash)->getValue();
                    if (cached_hash != 0) {
                        return cached_hash;
                    }
                }

                // get string's content which is typed `TypeArrayOop` and calculate hash value.
                typeArrayOop value_field = nullptr;
                if (string->getFieldValue(J_STRING, L"value", L"[C", (oop *) &value_field)) {
                    int length = value_field->getLength();
                    int hash = 0;
                    for (int i = 0; i < length; i++) {
                        auto single_char = (intOop) value_field->getElementAt(i);
                        hash = 31 * hash + single_char->getValue();
                    }
                    string->setFieldValue(hash_field, new intOopDesc(hash));
                    return hash;
                }

                return 0;
            }

            int String::Hash::operator()(const kivm::String &string) const noexcept {
                int hash = 0;
                for (wchar_t ch : string) {
                    hash = 31 * hash + (unsigned short) ch;
                }
                return hash;
            }

            bool String::EqualTo::operator()(instanceOop lhs, instanceOop rhs) const {
                if (lhs == rhs) {
                    return true;
                }
                if (lhs->getClass() != rhs->getClass()) {
                    return false;
                }

                auto klass = (InstanceKlass *) lhs->getClass();
                FieldID value_field = klass->getInstanceFieldInfo(J_STRING, L"value", L"[C");
                typeArrayOop lhs_value = nullptr;
                typeArrayOop rhs_value = nullptr;
                if (!lhs->getFieldValue(value_field, (oop *) &lhs_value)
                    || !rhs->getFieldValue(value_field, (oop *) &rhs_value)) {
                    return false;
                }

                int lhs_length = lhs_value->getLength();
                int rhs_length = rhs_value->getLength();
                if (lhs_length != rhs_length) {
                    return false;
                }

                for (int i = 0; i < lhs_length; ++i) {
                    auto lhs_char = (intOop) lhs_value->getElementAt(i);
                    auto rhs_char = (intOop) rhs_value->getElementAt(i);
                    if (lhs_char->getValue() != rhs_char->getValue()) {
                        return false;
                    }
                }
                return true;
            }
        }
    }
}
