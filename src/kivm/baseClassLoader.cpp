//
// Created by kiva on 2018/3/1.
//

#include <kivm/classLoader.h>
#include <kivm/oop/klass.h>
#include <kivm/oop/instanceKlass.h>
#include <kivm/oop/arrayKlass.h>
#include <kivm/classfile/classFileParser.h>
#include <sstream>

namespace kivm {
    ValueType parse_primitive_type(wchar_t c) {
        switch (c) {
            case L'B':    // byte
            case L'Z':    // boolean
            case L'S':    // short
            case L'C':    // char
            case L'I':    // int
                return ValueType::INT;
            case L'J':
                return ValueType::LONG;
            case L'F':    // float
                return ValueType::FLOAT;
            case L'D':    // double
                return ValueType::DOUBLE;
            default:
                PANIC("primitive type required");
                break;
        }
    }

    Klass *BaseClassLoader::loadClass(const String &class_name) {
        static const char *KLASSPATH_ENV = getenv("KLASSPATH");
        static String RT_DIR = KLASSPATH_ENV == nullptr
                               ? L""
                               : strings::from_std_string(KLASSPATH_ENV);

        // Load array class
        if (class_name[0] == L'[') {
            // [I
            int dimension = 0;
            while (class_name[++dimension] == L'[') continue;

            // Load 1-dimension array directly
            if (dimension == 1) {
                // for example: [Ljava/lang/Object;
                if (class_name[1] == L'L') {
                    // java/lang/Object
                    const String &component = class_name.substr(2, class_name.size() - 3);
                    auto *component_class = (InstanceKlass *) loadClass(component);
                    return component_class != nullptr
                           ? new ObjectArrayKlass(this, dimension, component_class)
                           : nullptr;
                }

                // for example: LI -> I
                ValueType component_type = parse_primitive_type(class_name[1]);
                return new TypeArrayKlass(this, dimension, component_type);
            }

            // Load multi-dimension array recursively
            // remove the first '['
            const String &down_type_name = class_name.substr(1);
            auto *down_type = (ArrayKlass *) loadClass(down_type_name);
            if (down_type == nullptr) {
                return nullptr;
            }

            if (down_type->is_object_array()) {
                return new ObjectArrayKlass(this, (ObjectArrayKlass *) down_type);
            } else {
                return new TypeArrayKlass(this, (TypeArrayKlass *) down_type);
            }
        }

        // Load instance class
        std::wstringstream path_builder;
        path_builder << RT_DIR << L'/' << class_name << L".class";
        const String &class_file_path = path_builder.str();
        ClassFileParser classFileParser(strings::to_std_string(class_file_path).c_str());
        ClassFile *classFile = classFileParser.classFile();
        return classFile != nullptr
               ? new InstanceKlass(classFile, this)
               : nullptr;
    }
}
