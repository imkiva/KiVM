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

    struct ClassSearchResult {
        ClassSource _source;
        u1 *_buffer;
        size_t _bufferSize;

        ClassSearchResult(ClassSource _source, u1 *_buffer, size_t _bufferSize)
            : _source(_source), _buffer(_buffer), _bufferSize(_bufferSize) {
        }
    };

    struct ClassPathEntry {
        ClassSource _source;
        String _path;
        void *_cookie;
    };

    class ClassPathManager {
    private:
        std::list<ClassPathEntry> _runtimeClassPath;

    public:
        static ClassPathManager *get();

        void addClassPath(const String &path);

        ClassSearchResult searchClass(const String &className);
    };
}
