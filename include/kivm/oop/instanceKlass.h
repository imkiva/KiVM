//
// Created by kiva on 2018/2/25.
//
#pragma once

#include <kivm/oop/klass.h>
#include <kivm/oop/oopfwd.h>
#include <kivm/oop/reflectionSupport.h>
#include <kivm/runtime/constantPool.h>
#include <unordered_map>
#include <vector>

namespace kivm {
    class Method;

    class Field;

    class InnerClasses_attribute;

    class EnclosingMethod_attribute;

    class BootstrapMethods_attribute;

    class InstanceKlass : public Klass {
        friend class instanceOopDesc;

    private:
        ClassLoader *_class_loader;
        mirrorOop _java_loader;

        ClassFile *_class_file;
        String _source_file;
        String _signature;

        InnerClasses_attribute *_ic_attr;
        EnclosingMethod_attribute *_em_attr;
        BootstrapMethods_attribute *_bm_attr;

        RuntimeConstantPool _runtime_pool;

        /**
         * virtual methods (public or protected methods).
         * map<name + " " + descriptor, method>
         */
        std::unordered_map<String, Method *> _vtable;

        /**
         * private or final methods.
         * map<name + " " + descriptor, method>
         */
        std::unordered_map<String, Method *> _pftable;

        /**
         * static methods.
         * map<name + " " + descriptor, method>
         */
        std::unordered_map<String, Method *> _stable;

        /**
         * static fields.
         * map<className + " " + name + " " + descriptor, <vector-offset, Field*>>
         */
        std::unordered_map<String, FieldID> _static_fields;

        /**
         * instance fields.
         * map<className + " " + name + " " + descriptor, <vector-offset, Field*>>
         */
        std::unordered_map<String, FieldID> _instance_fields;

        int _n_static_fields;

        int _n_instance_fields;

        /**
         * static fields' values.
         */
        std::vector<oop> _static_field_values;

        /**
         * interfaces
         * map<interface-name, class>
         */
        std::unordered_map<String, InstanceKlass *> _interfaces;

    private:
        InstanceKlass *requireInstanceClass(u2 class_info_index);

        void link_constant_pool(cp_info **constant_pool);

        void link_super_class(cp_info **pool);

        void link_methods(cp_info **pool);

        void link_interfaces(cp_info **pool);

        void link_fields(cp_info **pool);

        void link_attributes(cp_info **pool);

    public:
        InstanceKlass(ClassFile *class_file, ClassLoader *class_loader,
                      mirrorOop java_loader, ClassType class_type);

        ClassLoader *getClassLoader() const {
            return _class_loader;
        }

        const String &getSourceFile() const {
            return _source_file;
        }

        const String &getSignature() const {
            return _signature;
        }

        RuntimeConstantPool &getRuntimeConstantPool() {
            return this->_runtime_pool;
        }

        void linkAndInit() override;

        const std::unordered_map<String, Method *> &getVtable() const {
            return _vtable;
        }

        /**
         * Get static field offset.
         * @param className Where the wanted field belongs to
         * @param name Field name
         * @param descriptor Field descriptor
         * @return vector offset if found, otherwise -1
         */
        int getStaticFieldOffset(const String &className,
                                 const String &name,
                                 const String &descriptor) const;

        /**
         * Get static field info.
         * @param className Where the wanted field belongs to
         * @param name Field name
         * @param descriptor Field descriptor
         * @return FieldID if found, otherwise {@code FieldID(-1, nullptr)}
         */
        FieldID getStaticFieldInfo(const String &className,
                                   const String &name,
                                   const String &descriptor) const;

        /**
         * Get instance field offset.
         * @param className Where the wanted field belongs to
         * @param name Field name
         * @param descriptor Field descriptor
         * @return vector offset if found, otherwise -1
         */
        int getInstanceFieldOffset(const String &className,
                                   const String &name,
                                   const String &descriptor) const;

        /**
         * Get instance field info.
         * @param className Where the wanted field belongs to
         * @param name Field name
         * @param descriptor Field descriptor
         * @return FieldID if found, otherwise {@code FieldID(-1, nullptr)}
         */
        FieldID getInstanceFieldInfo(const String &className,
                                     const String &name,
                                     const String &descriptor) const;

        /**
         * Get virtual method.
         * @param name Method name
         * @param descriptor Method descriptor
         * @return method pointer if found, otherwise {@code nullptr}
         */
        Method *findVirtualMethod(const String &name, const String &descriptor) const;

        /**
         * Get non-virtual method(private or final).
         * @param name Method name
         * @param descriptor Method descriptor
         * @return method pointer if found, otherwise {@code nullptr}
         */
        Method *findNonVirtualMethod(const String &name, const String &descriptor) const;

        /**
         * Get static method.
         * @param name Method name
         * @param descriptor Method descriptor
         * @return method pointer if found, otherwise {@code nullptr}
         */
        Method *findStaticMethod(const String &name, const String &descriptor) const;

        /**
         * Get interface.
         * @param interfaceClassName Interface class name
         * @return Interface class representation if found, otherwise {@code nullptr}
         */
        InstanceKlass *findInterface(const String &interfaceClassName) const;

        /**
         * Set static field's value.
         * @param className Where the wanted field belongs to
         * @param name Field name
         * @param descriptor Field descriptor
         * @param value field value
         */
        void setStaticFieldValue(const String &className,
                                 const String &name,
                                 const String &descriptor,
                                 oop value);

        /**
         * Set static field's value.
         * @param fieldID field descriptor
         * @param value field value
         */
        void setStaticFieldValue(const FieldID &fieldID,
                                 oop value);

        /**
         * Get static field's value.
         * @param className Where the wanted field belongs to
         * @param name Field name
         * @param descriptor Field descriptor
         * @param result pointer to result
         * @return {@code true} if found, otherwise {@code false}
         */
        bool getStaticFieldValue(const String &className,
                                 const String &name,
                                 const String &descriptor,
                                 oop *result);

        /**
         * Get static field's value.
         * @param fieldID field descriptor
         * @param result pointer to result
         * @return {@code true} if found, otherwise {@code false}
         */
        bool getStaticFieldValue(const FieldID &fieldID, oop *result);

        /**
         * Set instance field's value.
         * @param receiver Java object that contains the wanted field
         * @param className Where the wanted field belongs to
         * @param name Field name
         * @param descriptor Field descriptor
         * @param value field value
         */
        void setInstanceFieldValue(instanceOop receiver,
                                   const String &className,
                                   const String &name,
                                   const String &descriptor,
                                   oop value);

        /**
         * Set static field's value.
         * @param receiver Java object that contains the wanted field
         * @param fieldID field descriptor
         * @param value field value
         */
        void setInstanceFieldValue(instanceOop receiver, const FieldID &fieldID, oop value);

        /**
         * Get instance field's value.
         * @param receiver Java object that contains the wanted field
         * @param className Where the wanted field belongs to
         * @param name Field name
         * @param descriptor Field descriptor
         * @param result pointer to result
         * @return {@code true} if found, otherwise {@code false}
         */
        bool getInstanceFieldValue(instanceOop receiver, const String &className,
                                   const String &name,
                                   const String &descriptor,
                                   oop *result);

        /**
         * Get instance field's value.
         * @param receiver Java object that contains the wanted field
         * @param fieldID field descriptor
         * @param result pointer to result
         * @return {@code true} if found, otherwise {@code false}
         */
        bool getInstanceFieldValue(instanceOop receiver, const FieldID &fieldID, oop *result);

        instanceOop newInstance();
    };
}
