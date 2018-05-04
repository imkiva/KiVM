//
// Created by kiva on 2018/3/31.
//

#include <kivm/runtime/constantPool.h>
#include <kivm/oop/instanceKlass.h>
#include <kivm/oop/arrayOop.h>

namespace kivm {
    RuntimeConstantPool::RuntimeConstantPool(InstanceKlass *instanceKlass)
        : _classLoader(instanceKlass->getClassLoader()) {
    }

    namespace pools {
        namespace impl {
            FieldPoolEntry getField(RuntimeConstantPool *rt, cp_info **pool, int index, bool isStatic) {
                auto fieldRef = (CONSTANT_Fieldref_info *) pool[index];
                Klass *klass = rt->getClass(fieldRef->class_index);

                if (klass->getClassType() == ClassType::INSTANCE_CLASS) {
                    auto instanceKlass = (InstanceKlass *) klass;
                    const auto &nameAndType = rt->getNameAndType(fieldRef->name_and_type_index);

                    if (isStatic) {
                        return instanceKlass->getThisClassField(nameAndType.first, nameAndType.second);

                    } else {
                        auto currentClass = instanceKlass;
                        while (currentClass != nullptr) {
                            auto found = currentClass->getInstanceFieldInfo(currentClass->getName(),
                                                                            nameAndType.first,
                                                                            nameAndType.second);
                            if (found != nullptr) {
                                return found;
                            }

                            // field not found in current method
                            // try superclass
                            currentClass = currentClass->getSuperClass();
                        }
                    }
                }
                PANIC("Unsupported field & class type.");
                return nullptr;
            }
        }

        FieldPoolEntry
        StaticFieldCreator::operator()(RuntimeConstantPool *rt, cp_info **pool, int index) {
            return impl::getField(rt, pool, index, true);
        }

        FieldPoolEntry
        InstanceFieldCreator::operator()(RuntimeConstantPool *rt, cp_info **pool, int index) {
            return impl::getField(rt, pool, index, false);
        }

        ClassPoolEntey ClassCreator::operator()(RuntimeConstantPool *rt, cp_info **pool, int index) {
            auto classInfo = (CONSTANT_Class_info *) pool[index];
            return BootstrapClassLoader::get()->loadClass(
                rt->getUtf8(classInfo->name_index));
        }

        StringPoolEntry StringCreator::operator()(RuntimeConstantPool *rt, cp_info **pool, int index) {
            auto classInfo = (CONSTANT_String_info *) pool[index];
            return java::lang::String::intern(
                rt->getUtf8(classInfo->string_index));
        }

        MethodPoolEntry MethodCreator::operator()(RuntimeConstantPool *rt, cp_info **pool, int index) {
            int tag = rt->getConstantTag(index);
            u2 classIndex = 0;
            u2 nameAndTypeIndex = 0;
            if (tag == CONSTANT_Methodref) {
                auto methodRef = (CONSTANT_Methodref_info *) pool[index];
                classIndex = methodRef->class_index;
                nameAndTypeIndex = methodRef->name_and_type_index;

            } else if (tag == CONSTANT_InterfaceMethodref) {
                auto interfaceMethodRef = (CONSTANT_InterfaceMethodref_info *) pool[index];
                classIndex = interfaceMethodRef->class_index;
                nameAndTypeIndex = interfaceMethodRef->name_and_type_index;

            } else {
                PANIC("Unsupported method & class type.");
            }

            Klass *klass = rt->getClass(classIndex);
            if (klass->getClassType() == ClassType::INSTANCE_CLASS) {
                auto instanceKlass = (InstanceKlass *) klass;
                const auto &nameAndType = rt->getNameAndType(nameAndTypeIndex);

                if (tag == CONSTANT_Methodref) {
                    // invokespecial and invokestatic
                    auto found = instanceKlass->getThisClassMethod(nameAndType.first, nameAndType.second);
                    if (found == nullptr) {
                        // invokevirtual
                        found = instanceKlass->getVirtualMethod(nameAndType.first, nameAndType.second);
                    }
                    return found;

                } else {
                    // invokeinterface
                    // interface methods are virtual methods
                    return instanceKlass->getVirtualMethod(nameAndType.first, nameAndType.second);
                }
            }
            return nullptr;
        }

        NameAndTypePoolEntry
        NameAndTypeCreator::operator()(RuntimeConstantPool *rt, cp_info **pool, int index) {
            auto nameAndType = (CONSTANT_NameAndType_info *) pool[index];
            return std::make_pair(rt->getUtf8(nameAndType->name_index),
                                  rt->getUtf8(nameAndType->descriptor_index));
        }
    }
}

