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

        std::vector<oop> _instanceFieldValues;

    private:
        explicit instanceOopDesc(instanceOop other)
            : oopDesc(getClass(), getMarkOop()->getOopType()),
              _instanceFieldValues(other->_instanceFieldValues) {
        }

    public:
        explicit instanceOopDesc(InstanceKlass *klass);

        ~instanceOopDesc() override = default;

        inline InstanceKlass *getInstanceClass() const {
            return (InstanceKlass *) getClass();
        }

        inline instanceOop shallowCopy() {
            return new instanceOopDesc(this);
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
        inline void setFieldValue(FieldID *fieldID, oop value) {
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
        inline bool getFieldValue(const String &className,
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
        inline bool getFieldValue(FieldID *fieldID, oop *result) {
            return getInstanceClass()->getInstanceFieldValue(this, fieldID, result);
        }

        /**
         * Mirrored from {@code InstanceKlass}
         * Get instance field's value.
         * @param receiver Java object that contains the wanted field
         * @param offset field offset
         * @param result pointer to result
         * @return {@code true} if found, otherwise {@code false}
         */
        inline bool getFieldValueUnsafe(int offset, oop *result) {
            return getInstanceClass()->getInstanceFieldValueUnsafe(this, offset, result);
        }
    };
}