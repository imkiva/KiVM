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

                auto klass = (InstanceKlass *) string->get_klass();
                FieldID hash_field = klass->get_instance_field_info(J_STRING, L"hash", L"I");
                if (string->get_field_value(hash_field, &int_oop_hash)) {
                    int cached_hash = ((intOop) int_oop_hash)->get_value();
                    if (cached_hash != 0) {
                        return cached_hash;
                    }
                }

                // get string's content which is typed `TypeArrayOop` and calculate hash value.
                typeArrayOop value_field = nullptr;
                if (string->get_field_value(J_STRING, L"value", L"[C", (oop *) &value_field)) {
                    int length = value_field->get_length();
                    int hash = 0;
                    for (int i = 0; i < length; i++) {
                        auto single_char = (intOop) value_field->get_element_at(i);
                        hash = 31 * hash + single_char->get_value();
                    }
                    string->set_field_value(hash_field, new intOopDesc(hash));
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
                if (lhs->get_klass() != rhs->get_klass()) {
                    return false;
                }

                auto klass = (InstanceKlass *) lhs->get_klass();
                FieldID value_field = klass->get_instance_field_info(J_STRING, L"value", L"[C");
                typeArrayOop lhs_value = nullptr;
                typeArrayOop rhs_value = nullptr;
                if (!lhs->get_field_value(value_field, (oop *) &lhs_value)
                    || !rhs->get_field_value(value_field, (oop *) &rhs_value)) {
                    return false;
                }

                int lhs_length = lhs_value->get_length();
                int rhs_length = rhs_value->get_length();
                if (lhs_length != rhs_length) {
                    return false;
                }

                for (int i = 0; i < lhs_length; ++i) {
                    auto lhs_char = (intOop) lhs_value->get_element_at(i);
                    auto rhs_char = (intOop) rhs_value->get_element_at(i);
                    if (lhs_char->get_value() != rhs_char->get_value()) {
                        return false;
                    }
                }
                return true;
            }

            instanceOop String::intern(const kivm::String &string) {
                return StringTable::find_or_new(string);
            }

            instanceOop StringTable::find_or_new(const kivm::String &string) {
                static std::unordered_map<int, instanceOop> STRING_TABLE;

                // Find cached string oop
                int hash = String::Hash()(string);
                const auto &iter = STRING_TABLE.find(hash);
                if (iter != STRING_TABLE.end()) {
                    return iter->second;
                }

                // No cache, create new.
                auto *char_array_klass = (TypeArrayKlass *) BootstrapClassLoader::get()->loadClass(L"[C");
                auto *string_klass = (InstanceKlass *) BootstrapClassLoader::get()->loadClass(J_STRING);

                typeArrayOop chars = char_array_klass->new_instance((int) string.size());
                for (int i = 0; i < string.size(); ++i) {
                    chars->set_element_at(i, new intOopDesc((unsigned short) string[i]));
                }

                instanceOop java_string = string_klass->new_instance();
                java_string->set_field_value(J_STRING, L"value", L"[C", chars);
                STRING_TABLE.insert(std::make_pair(hash, java_string));
                return java_string;
            }
        }
    }
}
