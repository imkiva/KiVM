//
// Created by kiva on 2018/2/27.
//

#pragma once

#include <kivm/kivm.h>

namespace kivm {
    class Klass;

    class ClassLoader {
    public:
        static Klass *require_class(ClassLoader *classLoader, const String &className);

        virtual Klass *loadClass(const String &class_name) = 0;

        ClassLoader() = default;

        ClassLoader(const ClassLoader &) = delete;

        ClassLoader(ClassLoader &&) noexcept = delete;

        virtual ~ClassLoader() = default;
    };

    class BootstrapClassLoader : public ClassLoader {
    public:
        static BootstrapClassLoader *get();

        Klass *loadClass(const String &class_name) override;
    };
}
