//
// Created by kiva on 2018/3/31.
//

#include <kivm/runtime/constantPool.h>
#include <kivm/oop/instanceKlass.h>
#include <kivm/native/class_names.h>
#include <kivm/native/java_lang_String.h>
#include <kivm/oop/arrayOop.h>
#include <kivm/oop/primitiveOop.h>

namespace kivm {
    RuntimeConstantPool::RuntimeConstantPool(InstanceKlass *instanceKlass, cp_info **pool)
        : _class_loader(instanceKlass->getClassLoader()), _constant_pool(pool) {
    }

    instanceOop StringTable::findOrNew(const kivm::String &string) {
        static std::unordered_map<int, instanceOop> STRING_TABLE;

        // Find cached string oop
        int hash = java::lang::String::Hash()(string);
        const auto &iter = STRING_TABLE.find(hash);
        if (iter != STRING_TABLE.end()) {
            return iter->second;
        }

        // No cache, create new.
        auto *char_array_klass = (TypeArrayKlass *) BootstrapClassLoader::get()->loadClass(L"[C");
        auto *string_klass = (InstanceKlass *) BootstrapClassLoader::get()->loadClass(J_STRING);

        typeArrayOop chars = char_array_klass->newInstance((int) string.size());
        for (int i = 0; i < string.size(); ++i) {
            chars->setElementAt(i, new intOopDesc((unsigned short) string[i]));
        }

        instanceOop java_string = string_klass->newInstance();
        java_string->setFieldValue(J_STRING, L"value", L"[C", chars);
        STRING_TABLE.insert(std::make_pair(hash, java_string));
        return java_string;
    }
}

