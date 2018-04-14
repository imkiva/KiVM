//
// Created by kiva on 2018/3/1.
//

#include <kivm/classLoader.h>
#include <kivm/oop/klass.h>
#include <kivm/oop/instanceKlass.h>
#include <kivm/oop/arrayKlass.h>
#include <kivm/oop/reflectionSupport.h>
#include <kivm/classfile/classFileParser.h>
#include <sstream>

namespace kivm {
    Klass *BaseClassLoader::loadClass(const String &className) {
        static const char *KLASSPATH_ENV = getenv("KLASSPATH");
        static String RT_DIR = KLASSPATH_ENV == nullptr
                               ? L""
                               : strings::fromStdString(KLASSPATH_ENV);

        // Load array class
        if (className[0] == L'[') {
            // [I
            int dimension = 0;
            while (className[++dimension] == L'[') continue;

            // Load 1-dimension array directly
            if (dimension == 1) {
                // for example: [Ljava/lang/Object;
                if (className[1] == L'L') {
                    // java/lang/Object
                    const String &component = className.substr(2, className.size() - 3);
                    auto *component_class = (InstanceKlass *) loadClass(component);
                    return component_class != nullptr
                           ? new ObjectArrayKlass(this, nullptr, dimension, component_class)
                           : nullptr;
                }

                // for example: LI -> I
                ValueType component_type = primitiveTypeToValueType(className[1]);
                return new TypeArrayKlass(this, nullptr, dimension, component_type);
            }

            // Load multi-dimension array recursively
            // remove the first '['
            const String &down_type_name = className.substr(1);
            auto *down_type = (ArrayKlass *) loadClass(down_type_name);
            if (down_type == nullptr) {
                return nullptr;
            }

            if (down_type->isObjectArray()) {
                return new ObjectArrayKlass(this, (ObjectArrayKlass *) down_type);
            } else {
                return new TypeArrayKlass(this, (TypeArrayKlass *) down_type);
            }
        }

        // Load instance class
        std::wstringstream path_builder;
        path_builder << RT_DIR << L'/' << className << L".class";
        const String &class_file_path = path_builder.str();
        ClassFileParser classFileParser(strings::toStdString(class_file_path).c_str());
        ClassFile *classFile = classFileParser.getParsedClassFile();
        return classFile != nullptr
               ? new InstanceKlass(classFile, this, nullptr, ClassType::INSTANCE_CLASS)
               : nullptr;
    }
}
