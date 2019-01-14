//
// Created by kiva on 2019-01-15.
//
#pragma once

#include <compileTimeConfig.h>
#include <shared/string.h>

#if defined(KIVM_PLATFORM_UNIX) || defined(KIVM_PLATFORM_APPLE)
namespace kivm {
    class UnixFileSystem {
    public:
        static bool isDirectory(const String &path);

        static bool canRead(const String &path);

        static size_t getFileSize(const String &path);

        static void *createFileMapping(const String &path, int *pFd, size_t *pSize);

        static void destroyFileMapping(void *memory, int fd, size_t size);
    };
}
#endif

