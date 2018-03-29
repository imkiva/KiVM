//
// Created by kiva on 2018/3/29.
//

#include <kivm/native/class_names.h>
#include <kivm/native/java_lang_String.h>
#include <kivm/oop/instanceOop.h>
#include <kivm/oop/primitiveOop.h>
#include <kivm/oop/arrayOop.h>

namespace kivm {
    namespace java {
        namespace lang {
            size_t String::Hash::operator()(instanceOop oop) const {
                // if has a hash_val cache, need no calculate.
                oop int_oop_hash = nullptr;

                auto klass = (InstanceKlass *) oop->get_klass();
                FieldID hash_field = klass->get_instance_field_info(J_STRING, L"hash", L"I");
                if (oop->get_field_value(hash_field, &int_oop_hash)) {
                    int cached_value = ((intOop) int_oop_hash)->get_value();
                    if (cached_value != 0) {
                        return static_cast<size_t>(cached_value);
                    }
                }

                // get string's content which is typed `TypeArrayOop` and calculate hash value.
                typeArrayOop value_field = nullptr;
                if (oop->get_field_value(J_STRING, L"value", L"[C", &value_field)) {
                    int length = value_field->get_length();
                    int hash = 0;
                    for (int i = 0; i < length; i++) {
                        auto single_char = (intOop) value_field->get_element_at(i);
                        hash = 31 * hash + single_char->get_value();
                    }
                    oop->set_field_value(hash_field, new intOopDesc(hash));
                    return static_cast<size_t>(hash);
                }

                return 0;
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
                if (!lhs->get_field_value(value_field, &lhs_value)
                    || !rhs->get_field_value(value_field, &rhs_value)) {
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
        }
    }
}
