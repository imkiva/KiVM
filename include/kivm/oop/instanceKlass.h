//
// Created by kiva on 2018/2/25.
//
#pragma once

#include <kivm/oop/klass.h>
#include <kivm/oop/oopfwd.h>
#include <kivm/oop/reflection.h>
#include <kivm/runtime/constantPool.h>
#include <shared/monitor.h>
#include <sparsepp/spp.h>
#include <vector>

namespace kivm {
    class Method;

    class Field;

    class InnerClasses_attribute;

    class EnclosingMethod_attribute;

    class BootstrapMethods_attribute;

    class InstanceKlass : public Klass {
        friend class instanceOopDesc;

        friend class CopyingHeap;

    private:
        ClassLoader *_classLoader;
        mirrorOop _javaLoader;

        ClassFile *_classFile;
        String _sourceFile;
        String _signature;

        InnerClasses_attribute *_innerClassAttr;
        EnclosingMethod_attribute *_enclosingMethodAttr;
        BootstrapMethods_attribute *_bootstrapMethodAttr;

        RuntimeConstantPool *_runtimePool;

        int _nStaticFields;

        int _nInstanceFields;

        /**
         * all methods in this class.
         * map<name + " " + descriptor, method>
         */
        spp::sparse_hash_map<String, MethodID *> _allMethods;

        /**
         * virtual methods (public or protected methods).
         * map<name + " " + descriptor, method>
         */
        spp::sparse_hash_map<String, MethodID *> _vtable;

        /**
         * static fields.
         * map<className + " " + name + " " + descriptor, <vector-offset, Field*>>
         */
        spp::sparse_hash_map<String, FieldID *> _staticFields;

        /**
         * instance fields.
         * map<className + " " + name + " " + descriptor, <vector-offset, Field*>>
         */
        spp::sparse_hash_map<String, FieldID *> _instanceFields;

        /**
         * static fields' values.
         */
        std::vector<oop> _staticFieldValues;

        /**
         * interfaces
         * map<interface-name, class>
         */
        spp::sparse_hash_map<String, InstanceKlass *> _interfaces;

    private:
        InstanceKlass *requireInstanceClass(u2 classInfoIndex);

        void linkConstantPool(cp_info **pool);

        void linkSuperClass(cp_info **pool);

        void linkMethods(cp_info **pool);

        void linkInterfaces(cp_info **pool);

        void linkFields(cp_info **pool);

        void linkAttributes(cp_info **pool);

    public:
        InstanceKlass(ClassFile *classFile, ClassLoader *classLoader,
                      mirrorOop javaLoader, ClassType classType);

        inline ClassLoader *getClassLoader() const {
            return _classLoader;
        }

        inline const String &getSourceFile() const {
            return _sourceFile;
        }

        inline const String &getSignature() const {
            return _signature;
        }

        inline RuntimeConstantPool *getRuntimeConstantPool() {
            return _runtimePool;
        }

        void linkClass() override;

        void initClass() override;

        inline const spp::sparse_hash_map<String, FieldID *> &getStaticFields() {
            return _staticFields;
        }

        inline const spp::sparse_hash_map<String, FieldID *> &getInstanceFields() {
            return _instanceFields;
        }

        inline const spp::sparse_hash_map<String, MethodID *> &getDeclaredMethods() {
            return _allMethods;
        }

        Method *getDeclaredMethodByOffset(int offset);

        /**
         * Search field in this class.
         * @param name Field name
         * @param descriptor Field descriptor
         * @return FieldID if found, otherwise {@code FieldID(-1, nullptr)}
         */
        FieldID *getThisClassField(const String &name,
                                   const String &descriptor) const;

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
        FieldID *getStaticFieldInfo(const String &className,
                                    const String &name,
                                    const String &descriptor) const;

        /**
         * Get instance field info.
         * @param className Where the wanted field belongs to
         * @param name Field name
         * @param descriptor Field descriptor
         * @return FieldID if found, otherwise {@code FieldID(-1, nullptr)}
         */
        FieldID *getInstanceFieldInfo(const String &className,
                                      const String &name,
                                      const String &descriptor) const;

        /**
         * Search method in this class.
         * @param name Method name
         * @param descriptor Method descriptor
         * @return method pointer if found, otherwise {@code nullptr}
         */
        Method *getThisClassMethod(const String &name, const String &descriptor) const;

        /**
         * Get virtual method.
         * @param name Method name
         * @param descriptor Method descriptor
         * @return method pointer if found, otherwise {@code nullptr}
         */
        Method *getVirtualMethod(const String &name, const String &descriptor) const;

        /**
         * Get static method.
         * @param name Method name
         * @param descriptor Method descriptor
         * @return method pointer if found, otherwise {@code nullptr}
         */
        Method *getStaticMethod(const String &name, const String &descriptor) const;

        /**
         * Get interface.
         * @param interfaceClassName Interface class name
         * @return Interface class representation if found, otherwise {@code nullptr}
         */
        InstanceKlass *getInterface(const String &interfaceClassName) const;

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
        void setStaticFieldValue(FieldID *fieldID,
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
        bool getStaticFieldValue(FieldID *fieldID, oop *result);

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
        void setInstanceFieldValue(instanceOop receiver, FieldID *fieldID, oop value);

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
        bool getInstanceFieldValue(instanceOop receiver, FieldID *fieldID, oop *result);

        /**
         * Get instance field's value.
         * @param receiver Java object that contains the wanted field
         * @param offset field offset
         * @param result pointer to result
         * @return {@code true} if found, otherwise {@code false}
         */
        bool getInstanceFieldValueUnsafe(instanceOop receiver, int offset, oop *result);

        instanceOop newInstance();

        bool checkInterface(InstanceKlass *interfaceClass);
    };
}
