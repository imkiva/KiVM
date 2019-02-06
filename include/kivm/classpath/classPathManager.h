//
// Created by kiva on 2018/4/21.
//
#pragma once

#include <kivm/kivm.h>
#include <list>

namespace kivm {
    enum ClassSource {
        NOT_FOUND,
        DIR,
        JAR
    };

    struct ClassSearchResult final {
        String _file;
        int _fd{};
        ClassSource _source;
        u1 *_buffer = nullptr;
        size_t _bufferSize{};

        ClassSearchResult(const String &file, int fd, ClassSource source, u1 *buffer, size_t bufferSize);

        void closeResource() const;
    };

    struct ClassPathEntry final {
        ClassSource _source;
        String _path;
        void *_cookie;
    };

    class ClassPathManager final {
    private:
        std::list<ClassPathEntry> _runtimeClassPath;

    public:
        static void initialize();

        static ClassPathManager *get();

        void addClassPath(const String &path);

        void addClassPaths(const String &classpath);

        ClassSearchResult searchClass(const String &className);

        void destroy();
    };
}
