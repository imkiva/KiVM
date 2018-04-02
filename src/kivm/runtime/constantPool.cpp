//
// Created by kiva on 2018/3/31.
//

#include <kivm/runtime/constantPool.h>
#include <kivm/oop/instanceKlass.h>
#include <kivm/oop/arrayOop.h>

namespace kivm {
    RuntimeConstantPool::RuntimeConstantPool(InstanceKlass *instanceKlass)
        : _class_loader(instanceKlass->getClassLoader()) {
    }

    static String getUtf8(cp_info **pool, int utf8Index) {
        auto *utf8 = (CONSTANT_Utf8_info *) pool[utf8Index];
        return utf8->get_constant();
    }

    static std::pair<const String &, const String &>
    getNameAndType(cp_info **pool, int nameAndTypeIndex) {
        auto nameAndType = (CONSTANT_NameAndType_info *) pool[nameAndTypeIndex];
        return std::make_pair(getUtf8(pool, nameAndType->name_index),
                              getUtf8(pool, nameAndType->descriptor_index));
    }

    Klass *pools::ClassCreator::operator()(RuntimeConstantPool *rt, cp_info **pool, int index) {
        auto classInfo = (CONSTANT_Class_info *) pool[index];
        return BootstrapClassLoader::get()->loadClass(
            getUtf8(pool, classInfo->name_index));
    }

    instanceOop pools::StringCreator::operator()(RuntimeConstantPool *rt, cp_info **pool, int index) {
        auto classInfo = (CONSTANT_String_info *) pool[index];
        return java::lang::String::from(
            getUtf8(pool, classInfo->string_index));
    }

    Method *pools::MethodCreator::operator()(RuntimeConstantPool *rt, cp_info **pool, int index) {
        auto methodRef = (CONSTANT_Methodref_info *) pool[index];
        Klass *klass = rt->get_class(methodRef->class_index);
        if (klass->getClassType() == ClassType::INSTANCE_CLASS) {
            auto instanceKlass = (InstanceKlass *) klass;
            const auto &nameAndType = getNameAndType(
                pool, methodRef->name_and_type_index);
            return instanceKlass->findThisClassMethod(nameAndType.first, nameAndType.second);
        }
        PANIC("Unsupported method & class type.");
        return nullptr;
    }

    Field *pools::FieldCreator::operator()(RuntimeConstantPool *rt, cp_info **pool, int index) {
        return nullptr;
    }
}

