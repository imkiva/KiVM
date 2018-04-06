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

        inline InstanceKlass *getInstanceClass() const {
            return (InstanceKlass *) getClass();
        }

        /**
         * Mirrored from {@code InstanceKlass}
         * Set instance field's value.
         * @param className Where the wanted field belongs to
         * @param name Field name
         * @param descriptor Field descriptor
         * @param value field value
         */
        inline void setFieldValue(const String &className,
                                  const String &name,
                                  const String &descriptor,
                                  oop value) {
            getInstanceClass()->setInstanceFieldValue(this, className, name, descriptor, value);
        }

        /**
         * Mirrored from {@code InstanceKlass}
         * Set static field's value.
         * @param fieldID field descriptor
         * @param value field value
         */
        void setFieldValue(FieldID *fieldID, oop value) {
            getInstanceClass()->setInstanceFieldValue(this, fieldID, value);
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
        bool getFieldValue(const String &className,
                           const String &name,
                           const String &descriptor,
                           oop *result) {
            return getInstanceClass()
                ->getInstanceFieldValue(this, className, name, descriptor, result);
        }

        /**
         * Mirrored from {@code InstanceKlass}
         * Get instance field's value.
         * @param fieldID field descriptor
         * @param result pointer to result
         * @return {@code true} if found, otherwise {@code false}
         */
        bool getFieldValue(FieldID *fieldID, oop *result) {
            return getInstanceClass()->getInstanceFieldValue(this, fieldID, result);
        }
    };
}