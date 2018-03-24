//
// Created by kiva on 2018/2/25.
//
#pragma once

#include <kivm/oop/klass.h>
#include <kivm/oop/oopfwd.h>
#include <kivm/oop/reflection_support.h>
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
        mirrorOop _mirror_loader;

        ClassFile *_class_file;
        String _source_file;
        String _signature;

        InnerClasses_attribute *_ic_attr;
        EnclosingMethod_attribute *_em_attr;
        BootstrapMethods_attribute *_bm_attr;

        /**
         * Methods that have been invoked by invokevirtual.
         * map<constant-index, method>
         */
        std::unordered_map<u2, Method *> _vtable_cache;

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

        InstanceKlass *require_instance_class(u2 class_info_index);

        void link_constant_pool(cp_info **constant_pool);

        void link_super_class(cp_info **pool);

        void link_methods(cp_info **pool);

        void link_interfaces(cp_info **pool);

        void link_fields(cp_info **pool);

        void link_attributes(cp_info **pool);

    public:
        InstanceKlass(ClassFile *classFile, ClassLoader *class_loader);

        ClassLoader *get_class_loader() const {
            return _class_loader;
        }

        const String &get_source_file() const {
            return _source_file;
        }

        const String &get_signature() const {
            return _signature;
        }

        void link_and_init() override;

        const std::unordered_map<String, Method *> &get_vtable() const {
            return _vtable;
        }

        /**
         * Get static field offset.
         * @param className Where the wanted field belongs to
         * @param name Field name
         * @param descriptor Field descriptor
         * @return vector offset if found, otherwise -1
         */
        int get_static_field_offset(const String &className,
                                    const String &name,
                                    const String &descriptor) const;

        /**
         * Get static field info.
         * @param className Where the wanted field belongs to
         * @param name Field name
         * @param descriptor Field descriptor
         * @return FieldID if found, otherwise {@code FieldID(-1, nullptr)}
         */
        FieldID get_static_field_info(const String &className,
                                      const String &name,
                                      const String &descriptor) const;

        /**
         * Get instance field offset.
         * @param className Where the wanted field belongs to
         * @param name Field name
         * @param descriptor Field descriptor
         * @return vector offset if found, otherwise -1
         */
        int get_instance_field_offset(const String &className,
                                      const String &name,
                                      const String &descriptor) const;

        /**
         * Get instance field info.
         * @param className Where the wanted field belongs to
         * @param name Field name
         * @param descriptor Field descriptor
         * @return FieldID if found, otherwise {@code FieldID(-1, nullptr)}
         */
        FieldID get_instance_field_info(const String &className,
                                        const String &name,
                                        const String &descriptor) const;

        /**
         * Get virtual method.
         * @param name Method name
         * @param descriptor Method descriptor
         * @return method pointer if found, otherwise {@code nullptr}
         */
        Method *find_virtual_method(const String &name, const String &descriptor) const;

        /**
         * Get non-virtual method(private or final).
         * @param name Method name
         * @param descriptor Method descriptor
         * @return method pointer if found, otherwise {@code nullptr}
         */
        Method *find_non_virtual_method(const String &name, const String &descriptor) const;

        /**
         * Get static method.
         * @param name Method name
         * @param descriptor Method descriptor
         * @return method pointer if found, otherwise {@code nullptr}
         */
        Method *find_static_method(const String &name, const String &descriptor) const;

        /**
         * Get interface.
         * @param interface_class_name Interface class name
         * @return Interface class representation if found, otherwise {@code nullptr}
         */
        InstanceKlass *find_interface(const String &interface_class_name) const;

        void set_static_field_value(const String &className,
                                    const String &name,
                                    const String &descriptor,
                                    oop value);

        void set_static_field_value(const FieldID &fieldID,
                                    oop value);

        oop get_static_field_value(const String &className,
                                   const String &name,
                                   const String &descriptor);

        oop get_static_field_value(const FieldID &fieldID);

        void set_instance_field_value(oop receiver,
                                      const String &className,
                                      const String &name,
                                      const String &descriptor,
                                      oop value);

        void set_instance_field_value(oop receiver, const FieldID &fieldID, oop value);

        oop get_instance_field_value(oop receiver, const String &className,
                                     const String &name,
                                     const String &descriptor);

        oop get_instance_field_value(oop receiver, const FieldID &fieldID);
    };
}
