//
// Created by kiva on 2018/2/28.
//
#pragma once
#include <kivm/oop/oop.h>
#include <kivm/oop/instanceKlass.h>
#include <vector>

namespace kivm {
    class instanceOopDesc : public oopDesc {
        friend class InstanceKlass;

        std::vector<oop> _instance_field_values;

    public:
        explicit instanceOopDesc(InstanceKlass *klass);

        ~instanceOopDesc() override = default;

        inline InstanceKlass *get_instance_class() const {
            return (InstanceKlass *) get_klass();
        }

        /**
         * Mirrored from {@code InstanceKlass}
         * Set instance field's value.
         * @param className Where the wanted field belongs to
         * @param name Field name
         * @param descriptor Field descriptor
         * @param value field value
         */
        inline void set_field_value(const String &className,
                                             const String &name,
                                             const String &descriptor,
                                             oop value) {
            get_instance_class()->set_instance_field_value(this, className, name, descriptor, value);
        }

        /**
         * Mirrored from {@code InstanceKlass}
         * Set static field's value.
         * @param fieldID field descriptor
         * @param value field value
         */
        void set_field_value(const FieldID &fieldID, oop value) {
            get_instance_class()->set_instance_field_value(this, fieldID, value);
        }

        /**
         * Mirrored from {@code InstanceKlass}
         * Get instance field's value.
         * @param className Where the wanted field belongs to
         * @param name Field name
         * @param descriptor Field descriptor
         * @param result pointer to result
         * @return {@code true} if found, otherwise {@code false}
         */
        bool get_field_value(const String &className,
                                      const String &name,
                                      const String &descriptor,
                                      oop *result) {
            return get_instance_class()
                    ->get_instance_field_value(this, className, name, descriptor, result);
        }

        /**
         * Mirrored from {@code InstanceKlass}
         * Get instance field's value.
         * @param fieldID field descriptor
         * @param result pointer to result
         * @return {@code true} if found, otherwise {@code false}
         */
        bool get_field_value(const FieldID &fieldID, oop *result) {
            return get_instance_class()->get_instance_field_value(this, fieldID, result);
        }
    };
}